/*
This is a dummy template that you could use to create a new metasound node.

 All code not associated with the Unreal Engine API: Copyright © 2025 Robert Esler
 */


//  Copyright © 2025 Epic Games, Inc. All rights reserved.

#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                         // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

//place your pd++ headers
#include "PdMaster.h"


// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_MetaSoundDummyNode"

namespace Metasound
{

    // Vertex Names - define your node's inputs and outputs here
    namespace DummyNodeNames
    {
        METASOUND_PARAM(InParamAudioInput, "In", "Audio Input")
        METASOUND_PARAM(DummyX, "A", "Dummy A");
        METASOUND_PARAM(DummyY, "B", "Dummy B");

        METASOUND_PARAM(OutParamAudio, "Out", "Audio output.")
    }


    // Operator Class - defines the way your node is described, created and executed
    class FDummyOperator : public TExecutableOperator<FDummyOperator>
    {
        public:
            // Constructor
            FDummyOperator(
                const FOperatorSettings& InSettings,
                const FAudioBufferReadRef& InAudioInput,
                const FFloatReadRef& InAValue,
                const FFloatReadRef& InBValue)
                : AudioInput(InAudioInput)
                , InputA(InAValue)
                , InputB(InBValue)
                , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
            {
            }

            // Helper function for constructing vertex interface
            static const FVertexInterface& DeclareVertexInterface()
            {
                using namespace DummyNodeNames;

                static const FVertexInterface Interface(
                        FInputVertexInterface( TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamAudioInput)),
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(DummyX)),
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(DummyY))
                    ),
                    FOutputVertexInterface(
                       
                        TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamAudio))
                    )
                );

                return Interface;
            }

            // Retrieves necessary metadata about your node
            static const FNodeClassMetadata& GetNodeInfo()
            {
                auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
                {
                    FVertexInterface NodeInterface = DeclareVertexInterface();

                    FNodeClassMetadata Metadata
                    {
                        FNodeClassName { StandardNodes::Namespace, "Dummy Node", StandardNodes::AudioVariant },
                        1, // Major Version
                        0, // Minor Version
                        METASOUND_LOCTEXT("DummyNodeDisplayName", "Dummy Node"),
                        METASOUND_LOCTEXT("DummyNodeDesc", "A Dummy Template"),
                        PluginAuthor,
                        PluginNodeMissingPrompt,
                        NodeInterface,
                        { }, // Category Hierarchy
                        { }, // Keywords for searching
                        FNodeDisplayStyle{}
                    };

                    return Metadata;
                };

                static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
                return Metadata;
            }

            // Allows MetaSound graph to interact with your node's inputs
            virtual FDataReferenceCollection GetInputs() const override
            {
                using namespace DummyNodeNames;

                FDataReferenceCollection InputDataReferences;
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamAudioInput), FAudioBufferReadRef(AudioInput));
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(DummyX), InputA);
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(DummyY), InputB);

                return InputDataReferences;
            }

            // Allows MetaSound graph to interact with your node's outputs
            virtual FDataReferenceCollection GetOutputs() const override
            {
                using namespace DummyNodeNames;

                FDataReferenceCollection OutputDataReferences;

                OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamAudio), FAudioBufferReadRef(AudioOutput));
                return OutputDataReferences;
            }

            // Used to instantiate a new runtime instance of your node
            static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
            {
                using namespace DummyNodeNames;

                const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();
                FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamAudioInput), InParams.OperatorSettings);
                TDataReadReference<float> InputA = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(DummyX), InParams.OperatorSettings);
                TDataReadReference<float> InputB = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(DummyY), InParams.OperatorSettings);

                return MakeUnique<FDummyOperator>(InParams.OperatorSettings, AudioIn, InputA, InputB);
            }

            // Primary node functionality
            void Execute()
            {
                const float* InputAudio = AudioInput->GetData();
                float* OutputAudio = AudioOutput->GetData();
                int32 NumFrames = AudioInput->Num();
                const float *temp = InputA.Get();
                const float *tempB = InputB.Get();
                //U E_LOG(LogTemp, Warning, TEXT("The float value is: %f"), *temp);
                for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
                {
                
                   // OutputAudio[FrameIndex] = osc.perform(InputAudio[FrameIndex]);
                    OutputAudio[FrameIndex] = InputAudio[FrameIndex];
                }
            }
        
        //This is a utility function based on Processing's map() method. 
        float map(float input, float rangeLow, float rangeHigh, float targetA, float targetB) {
            // Ensure the input is within bounds
            if (rangeLow == rangeHigh) return targetA; // Avoid division by zero

            // Normalize input to a 0-1 scale based on the source range
            double normalized = (input - rangeLow) / (rangeHigh - rangeLow);

            // Scale to the target range
            return targetA + normalized * (targetB - targetA);
        }

    private:

        // Inputs
        FAudioBufferReadRef AudioInput;
        FFloatReadRef InputA;//DummyX
        FFloatReadRef InputB;//DummyY
        
        // Outputs
        FAudioBufferWriteRef AudioOutput;
       
    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FDummyNode : public FNodeFacade
    {
        public:
            FDummyNode(const FNodeInitData& InitData)
                : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FDummyOperator>())
            {
            }
    };

    // Register node
    METASOUND_REGISTER_NODE(FDummyNode);
}

#undef LOCTEXT_NAMESPACE
