// Copyright Christopher Wratt 2024.
// All code under MIT license: see https://mit-license.org/

#pragma once

#include "AudioUtils.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FPowOperator
	//------------------------------------------------------------------------------------
	class FPowOperator : public TExecutableOperator<FPowOperator>
	{
	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors);

		FPowOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FAudioBufferReadRef& InPowerOf);

		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override;

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;
		FAudioBufferWriteRef AudioOutput;

		FAudioBufferReadRef	 mInPowerOf;

		DSPProcessing::FPow PowDSPProcessor;

	};

	//------------------------------------------------------------------------------------
	// FPowNode
	//------------------------------------------------------------------------------------
	class METASOUNDSAUDIOMATHUTILS_API FPowNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FPowNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FPowOperator>())
		{

		}
	};
}
