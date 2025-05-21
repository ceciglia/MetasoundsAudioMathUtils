#include "MetasoundPhasorNode.h"
#include <iostream>

#define LOCTEXT_NAMESPACE "MetasoundNodeTemplate_PhasorNode"

namespace Metasound
{
	namespace PhasorNode
	{
		// Input params
		METASOUND_PARAM(InParamNameAudioInput, "In", "Audio input.")

			// Output params
			METASOUND_PARAM(OutParamNameAudio, "Out", "Audio output.")
	}

	//------------------------------------------------------------------------------------
	// FPhasorOperator
	//------------------------------------------------------------------------------------
	FPhasorOperator::FPhasorOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput)
		: AudioInput(InAudioInput)
		, AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
	{
		sampleRate = InSettings.GetSampleRate();
		PhasorDSPProcessor.PrepareToPlay(sampleRate);
	}

	const FNodeClassMetadata& FPhasorOperator::GetNodeInfo()
	{
		auto InitNodeInfo = []() -> FNodeClassMetadata
			{
				FNodeClassMetadata Info;

				Info.ClassName = { TEXT("UE"), TEXT("Phasor (Audio)"), TEXT("Audio") };
				Info.MajorVersion = 1;
				Info.MinorVersion = 0;
				Info.DisplayName = LOCTEXT("Metasound_PhasorDisplayName", "Phasor(Audio)");
				Info.Description = LOCTEXT("Metasound_PhasorNodeDescription", "Generates a ramp between 0 and 1 (or viceversa) at a given frequency (signal in input)");
				Info.PromptIfMissing = PluginNodeMissingPrompt;
				Info.DefaultInterface = GetVertexInterface();
				Info.CategoryHierarchy = { LOCTEXT("Metasound_PhasorNodeCategory", "Utils") };

				return Info;
			};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FPhasorOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData)
	{
		using namespace PhasorNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
	}

	void FPhasorOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData)
	{
		using namespace PhasorNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);
	}

	const FVertexInterface& FPhasorOperator::GetVertexInterface()
	{
		using namespace PhasorNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput))
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FPhasorOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors)
	{
		using namespace PhasorNode;

		//const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
		const FDataReferenceCollection& InputCollection = InParams.InputData.ToDataReferenceCollection();
		const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

		FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

		return MakeUnique<FPhasorOperator>(InParams.OperatorSettings, AudioIn);
	}

	void FPhasorOperator::Execute()
	{
		const float* InputAudio = AudioInput->GetData();

		float* OutputAudio = AudioOutput->GetData();

		const int32 NumSamples = AudioInput->Num();

		PhasorDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, sampleRate, NumSamples);
	}

	METASOUND_REGISTER_NODE(FPhasorNode)
}

#undef LOCTEXT_NAMESPACE