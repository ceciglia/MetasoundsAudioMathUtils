#pragma once

#include "AudioUtils.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FPhasorOperator
	//------------------------------------------------------------------------------------
	class FPhasorOperator : public TExecutableOperator<FPhasorOperator>
	{
	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors);

		FPhasorOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput);

		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override;

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;

		FAudioBufferWriteRef AudioOutput;

		DSPProcessing::FPhasor PhasorDSPProcessor;

		int32 sampleRate;

	};

	//------------------------------------------------------------------------------------
	// FPhasorNode
	//------------------------------------------------------------------------------------
	class METASOUNDSAUDIOMATHUTILS_API FPhasorNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FPhasorNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FPhasorOperator>())
		{

		}
	};
}