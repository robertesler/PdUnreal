/*
This is a cricket model based on Andy Farnell's "Designing Sound"

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
#include "Cricket.cpp"


// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_MetaSoundCricketNode"

namespace Metasound
{

    // Vertex Names - define your node's inputs and outputs here
    namespace CricketNodeNames
    {
        METASOUND_PARAM(CricketX, "A", "Cricket Rate");
        METASOUND_PARAM(CricketY, "B", "Cricket Volue");

        METASOUND_PARAM(OutParamAudio, "Out", "Audio output.")
    }


    // Operator Class - defines the way your node is described, created and executed
    class FCricketOperator : public TExecutableOperator<FCricketOperator>
    {
        public:
            // Constructor
            FCricketOperator(
                const FOperatorSettings& InSettings,
                const FFloatReadRef& InAValue,
                const FFloatReadRef& InBValue)
                : InputA(InAValue)
                , InputB(InBValue)
                , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
            {
            }

            // Helper function for constructing vertex interface
            static const FVertexInterface& DeclareVertexInterface()
            {
                using namespace CricketNodeNames;

                static const FVertexInterface Interface(
                        FInputVertexInterface(
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(CricketX)),
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(CricketY))
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
                        FNodeClassName { StandardNodes::Namespace, "Cricket Node", StandardNodes::AudioVariant },
                        1, // Major Version
                        0, // Minor Version
                        METASOUND_LOCTEXT("CricketNodeDisplayName", "Cricket Node"),
                        METASOUND_LOCTEXT("CricketNodeDesc", "A Cricket Generator"),
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
                using namespace CricketNodeNames;

                FDataReferenceCollection InputDataReferences;
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(CricketX), InputA);
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(CricketY), InputB);

                return InputDataReferences;
            }

            // Allows MetaSound graph to interact with your node's outputs
            virtual FDataReferenceCollection GetOutputs() const override
            {
                using namespace CricketNodeNames;

                FDataReferenceCollection OutputDataReferences;

                OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamAudio), FAudioBufferReadRef(AudioOutput));
                return OutputDataReferences;
            }

            // Used to instantiate a new runtime instance of your node
            static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
            {
                using namespace CricketNodeNames;

                const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();
                TDataReadReference<float> InputA = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(CricketX), InParams.OperatorSettings);
                TDataReadReference<float> InputB = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(CricketY), InParams.OperatorSettings);

                return MakeUnique<FCricketOperator>(InParams.OperatorSettings, InputA, InputB);
            }

            // Primary node functionality
            void Execute()
            {
                float* OutputAudio = AudioOutput->GetData();
                int32 NumFrames = AudioOutput->Num();
                const float *tempA = InputA.Get();
                const float *tempB = InputB.Get();
                float rate = map(*tempA, 0, 100, 1, 2);
                float gain = map(*tempB, 0, 100, .2, 1);
                
                //U E_LOG(LogTemp, Warning, TEXT("The float value is: %f"), *rate);
                for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
                {
                    OutputAudio[FrameIndex] = cricket.perform(rate) * gain;
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
        FFloatReadRef InputA;//CricketX
        FFloatReadRef InputB;//CricketY
        
        // Outputs
        FAudioBufferWriteRef AudioOutput;
        CricketGen cricket;
       
    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FCricketNode : public FNodeFacade
    {
        public:
            FCricketNode(const FNodeInitData& InitData)
                : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FCricketOperator>())
            {
            }
    };

    // Register node
    METASOUND_REGISTER_NODE(FCricketNode);
}

#undef LOCTEXT_NAMESPACE
