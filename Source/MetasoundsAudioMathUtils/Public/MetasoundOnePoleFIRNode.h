// Copyright Christopher Wratt 2024.
// All code under MIT license: see https://mit-license.org/

#pragma once

#include "AudioUtils.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FOnePoleFIROperator
	//------------------------------------------------------------------------------------
	class FOnePoleFIROperator : public TExecutableOperator<FOnePoleFIROperator>
	{
	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors);

		FOnePoleFIROperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FAudioBufferReadRef& InCoefficientA, const FAudioBufferReadRef& InCoefficientB);

		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override;

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;
		FAudioBufferReadRef	 mCoefficientA;
		FAudioBufferReadRef	 mCoefficientB;
		FAudioBufferWriteRef AudioOutput;

		DSPProcessing::FOnePoleFIR OnePoleFIRDSPProcessor;
	};

	//------------------------------------------------------------------------------------
	// FOnePoleFIRNode
	//------------------------------------------------------------------------------------
	class METASOUNDSAUDIOMATHUTILS_API FOnePoleFIRNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FOnePoleFIRNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FOnePoleFIROperator>())
		{

		}
	};
}
