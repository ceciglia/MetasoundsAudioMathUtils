// Copyright Christopher Wratt 2024.
// All code under MIT license: see https://mit-license.org/

#include "MetasoundAudioCompareNode.h"

#define LOCTEXT_NAMESPACE "MetasoundNodeTemplate_AudioCompareNode"
#include "MetasoundEnumRegistrationMacro.h"

namespace Metasound
{
	DEFINE_METASOUND_ENUM_BEGIN(EAudioComparisonType, FEnumAudioCompareType, "AudioComparisonType")
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::Equals, "EqualsDescription", "Equals", "EqualsDescriptionTT", "True if A and B are equal."),
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::NotEquals, "NotEqualsDescriptioin", "Not Equals", "NotEqualsTT", "True if A and B are not equal."),
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::LessThan, "LessThanDescription", "Less Than", "LessThanTT", "True if A is less than B."),
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::GreaterThan, "GreaterThanDescription", "Greater Than", "GreaterThanTT", "True if A is greater than B."),
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::LessThanOrEquals, "LessThanOrEqualsDescription", "Less Than Or Equals", "LessThanOrEqualsTT", "True if A is less than or equal to B."),
		DEFINE_METASOUND_ENUM_ENTRY(EAudioComparisonType::GreaterThanOrEquals, "GreaterThanOrEqualsDescription", "Greater Than Or Equals", "GreaterThanOrEqualsTT", "True if A is greater than or equal to B."),
		DEFINE_METASOUND_ENUM_END()

	namespace CompareNode
	{
		// Input params
		METASOUND_PARAM(InParamNameAudioInput, "In", "Audio input.")
			METASOUND_PARAM(InParamNameCompareComparator, "Compare", "The value to test the input against")
			METASOUND_PARAM(InputCompareType, "Type", "How to compare A and B.");

			// Output params
			METASOUND_PARAM(OutParamNameAudio, "Out", "Audio output.")
	}

	//------------------------------------------------------------------------------------
	// FCompareOperator
	//------------------------------------------------------------------------------------
	FCompareOperator::FCompareOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FAudioBufferReadRef& InCompareComparator, FEnumAudioCompareTypeReadRef& InComparisonTypeReadRef)
		: AudioInput(InAudioInput)
		, AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
		, mInCompareComparator(InCompareComparator)
		, mComparisonType(InComparisonTypeReadRef)
	{
		
	}

	const FNodeClassMetadata& FCompareOperator::GetNodeInfo()
	{
		auto InitNodeInfo = []() -> FNodeClassMetadata
			{
				FNodeClassMetadata Info;

				Info.ClassName = { TEXT("UE"), TEXT("Compare (Audio)"), TEXT("Audio") };
				Info.MajorVersion = 1;
				Info.MinorVersion = 0;
				Info.DisplayName = LOCTEXT("Metasound_CompareDisplayName", "Compare (Audio)");
				Info.Description = LOCTEXT("Metasound_CompareNodeDescription", "Compares inputs based on comparator, outputs 1 if the true, 0 if false.");
				Info.Author = "Chris Wratt";
				Info.PromptIfMissing = PluginNodeMissingPrompt;
				Info.DefaultInterface = GetVertexInterface();
				Info.CategoryHierarchy = { LOCTEXT("Metasound_CompareNodeCategory", "Utils") };

				return Info;
			};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FCompareOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData)
	{
		using namespace CompareNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameCompareComparator), mInCompareComparator);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InputCompareType), mComparisonType);
	}

	void FCompareOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData)
	{
		using namespace CompareNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);
	}

	const FVertexInterface& FCompareOperator::GetVertexInterface()
	{
		using namespace CompareNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput)),
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameCompareComparator)),
				TInputDataVertex<FEnumAudioCompareType>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputCompareType), (int32)EAudioComparisonType::Equals)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FCompareOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors)
	{
		using namespace CompareNode;

		const FDataReferenceCollection& InputCollection = InParams.InputData.ToDataReferenceCollection();
		const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

		FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);
		FAudioBufferReadRef InCompareComparator = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameCompareComparator), InParams.OperatorSettings);
		FEnumAudioCompareTypeReadRef InComparison = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FEnumAudioCompareType>(InputInterface, METASOUND_GET_PARAM_NAME(InputCompareType), InParams.OperatorSettings);

		//FCompareOperator(FEnumAudioCompareTypeReadRef&& InComparisonTypeReadRef);
		return MakeUnique<FCompareOperator>(InParams.OperatorSettings, AudioIn, InCompareComparator, InComparison);
	}


	void FCompareOperator::Execute()
	{
		const float* InputAudio = AudioInput->GetData();
		float* OutputAudio = AudioOutput->GetData();
		const float* InputCompareComparator = mInCompareComparator->GetData();

		const int32 NumSamples = AudioInput->Num();

		switch (*mComparisonType)
		{
		case EAudioComparisonType::Equals:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] == InputCompareComparator[Index];
			break;

		case EAudioComparisonType::NotEquals:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] != InputCompareComparator[Index];
			break;

		case EAudioComparisonType::LessThan:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] < InputCompareComparator[Index];
			break;

		case EAudioComparisonType::GreaterThan:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] > InputCompareComparator[Index];
			break;

		case EAudioComparisonType::LessThanOrEquals:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] <= InputCompareComparator[Index];
			break;

		case EAudioComparisonType::GreaterThanOrEquals:
			for (int32 Index = 0; Index < NumSamples; ++Index) OutputAudio[Index] = InputAudio[Index] >= InputCompareComparator[Index];
			break;
		}
	}

	METASOUND_REGISTER_NODE(FCompareNode)
}

#undef LOCTEXT_NAMESPACE
