// Copyright Christopher Wratt 2024.
// All code under MIT license: see https://mit-license.org/

#pragma once

#include "AudioUtils.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FSqrtOperator
	//------------------------------------------------------------------------------------
	class FSqrtOperator : public TExecutableOperator<FSqrtOperator>
	{
	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors);

		FSqrtOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput);

		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override;

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;
		FAudioBufferWriteRef AudioOutput;

		DSPProcessing::FSqrt SqrtDSPProcessor;

	};

	//------------------------------------------------------------------------------------
	// FSqrtNode
	//------------------------------------------------------------------------------------
	class METASOUNDSAUDIOMATHUTILS_API FSqrtNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FSqrtNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSqrtOperator>())
		{

		}
	};
}
