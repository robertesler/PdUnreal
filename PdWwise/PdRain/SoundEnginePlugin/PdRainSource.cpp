/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#include "PdRainSource.h"
#include "../PdRainConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreatePdRainSource(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, PdRainSource());
}

AK::IAkPluginParam* CreatePdRainSourceParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, PdRainSourceParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(PdRainSource, AkPluginTypeSource, PdRainConfig::CompanyID, PdRainConfig::PluginID)

PdRainSource::PdRainSource()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

PdRainSource::~PdRainSource()
{
}

AKRESULT PdRainSource::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSourcePluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (PdRainSourceParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    m_durationHandler.Setup(m_pParams->RTPC.fDuration, in_pContext->GetNumLoops(), in_rFormat.uSampleRate);

    return AK_Success;
}

AKRESULT PdRainSource::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT PdRainSource::Reset()
{
    return AK_Success;
}

AKRESULT PdRainSource::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeSource;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void PdRainSource::Execute(AkAudioBuffer* out_pBuffer)
{
    m_durationHandler.SetDuration(m_pParams->RTPC.fDuration);
    m_durationHandler.ProduceBuffer(out_pBuffer);

	float temp = m_pParams->RTPC.fIntensity;
	float tempB = m_pParams->RTPC.fRainVol;

    //These ranges tend to work the best
    float rainIntensity = this->map(temp, 100, 0, .001, .1);
    float bpCf = this->map(temp, 0, 100, 1400, 900);
    float bpQ = this->map(temp, 0, 100, 2, 3);
    float rainVol = this->map(temp, 0, 100, 2, 4);
    float bpRainVol = this->map(tempB, 0, 100, 1, 3);
    m_rain.setRain(rainIntensity, rainVol, bpCf, bpQ, bpRainVol);

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();

    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);

        uFramesProduced = 0;
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            // Generate output here
           // *pBuf++ = AK_FLOAT_TO_SAMPLETYPE(m_oscillator.perform(200));
            *pBuf++ = AK_FLOAT_TO_SAMPLETYPE(m_rain.perform());
            ++uFramesProduced;
        }
    }
}

AkReal32 PdRainSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}

float PdRainSource::map(float input, float rangeLow, float rangeHigh, float targetA, float targetB) {
    // Ensure the input is within bounds
    if (rangeLow == rangeHigh) return targetA; // Avoid division by zero

    // Normalize input to a 0-1 scale based on the source range
    double normalized = (input - rangeLow) / (rangeHigh - rangeLow);

    // Scale to the target range
    return targetA + normalized * (targetB - targetA);
}
