//
//  PdAudio.cpp
//  Pd (Mac)
//
//  Created by Esler,Robert Wadhams on 5/29/25.
//  Copyright © 2025 Epic Games, Inc. All rights reserved.
//
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                         // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

//pd++ headers
#include "PdMaster.h"
#include "Oscillator.h"
#include "Rain.cpp"

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_MetaSoundRainNode"

namespace Metasound
{

    // Vertex Names - define your node's inputs and outputs here
    namespace RainNodeNames
    {
        METASOUND_PARAM(InParamAudioInput, "In", "Does Nothing")
        METASOUND_PARAM(RainX, "A", "Rain Intensity");
        METASOUND_PARAM(RainY, "B", "Rain Volume");

        METASOUND_PARAM(OutParamAudio, "Out", "Audio output.")
    }


    // Operator Class - defines the way your node is described, created and executed
    class FRainOperator : public TExecutableOperator<FRainOperator>
    {
        public:
            // Constructor
            FRainOperator(
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
                using namespace RainNodeNames;

                static const FVertexInterface Interface(
                        FInputVertexInterface( TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamAudioInput)),
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(RainX)),
                        TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(RainY))
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
                        FNodeClassName { StandardNodes::Namespace, "Rain Node", StandardNodes::AudioVariant },
                        1, // Major Version
                        0, // Minor Version
                        METASOUND_LOCTEXT("RainNodeDisplayName", "Rain Node"),
                        METASOUND_LOCTEXT("RainNodeDesc", "A simple rain generator"),
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
                using namespace RainNodeNames;

                FDataReferenceCollection InputDataReferences;
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamAudioInput), FAudioBufferReadRef(AudioInput));
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(RainX), InputA);
                InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(RainY), InputB);

                return InputDataReferences;
            }

            // Allows MetaSound graph to interact with your node's outputs
            virtual FDataReferenceCollection GetOutputs() const override
            {
                using namespace RainNodeNames;

                FDataReferenceCollection OutputDataReferences;

                OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamAudio), FAudioBufferReadRef(AudioOutput));
                return OutputDataReferences;
            }

            // Used to instantiate a new runtime instance of your node
            static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
            {
                using namespace RainNodeNames;

                const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();
                FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamAudioInput), InParams.OperatorSettings);
                TDataReadReference<float> InputA = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(RainX), InParams.OperatorSettings);
                TDataReadReference<float> InputB = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(RainY), InParams.OperatorSettings);

                return MakeUnique<FRainOperator>(InParams.OperatorSettings, AudioIn, InputA, InputB);
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
                float rainIntensity = map(*temp, 0, 100, .001, .1);
                float bpCf = map(*temp, 0, 100, 1400, 900);
                float bpQ = map(*temp, 0, 100, 2, 3);
                float rainVol = map(*tempB, 0, 100, 2, 4);
                float bpRainVol = map(*tempB, 0, 100, 1, 3);
                rain.setRain(rainIntensity, rainVol, bpCf, bpQ, bpRainVol);
                
                for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
                {
                    //OutputAudio[FrameIndex] = InputAudio[FrameIndex];
                   // OutputAudio[FrameIndex] = osc.perform(InputAudio[FrameIndex]);
                    OutputAudio[FrameIndex] = rain.perform();
                }
            }
        
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
        FFloatReadRef InputA;//RainX
        FFloatReadRef InputB;//RainY
        
        // Outputs
        FAudioBufferWriteRef AudioOutput;
        pd::Oscillator osc;//for testing only
        Rain rain;
    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FRainNode : public FNodeFacade
    {
        public:
            FRainNode(const FNodeInitData& InitData)
                : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FRainOperator>())
            {
            }
    };

    // Register node
    METASOUND_REGISTER_NODE(FRainNode);
}

#undef LOCTEXT_NAMESPACE
