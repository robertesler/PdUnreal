
/*
 This is an implementation of a FMOD plugin that uses
 the Pd4P3 example of Rain
 https://github.com/robertesler/Pd4P3/tree/main/examples/Nature/Rain
 See Rain.cpp for more information

 All code not associated with the FMOD API: Copyright © 2025 Robert Esler

 This example FMOD plugin is meant to use the same code from the PdUnreal project
 You should be able to import all necessary code and build your plugin.

 You may want to clone the Pd++ library, and add the files to your search path, or to your project
 https://bitbucket.org/resler/
 */

//Without this Windows will throw an error on build
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "fmod.hpp" //this is required, make sure you don't forget it
#include "../../../PdUnreal/PdPlugin/Private/Rain.cpp" // not sure why VS made me do this, but you probably won't have to

extern "C"
{
    // FMOD uses this to create the plugin in memory
    F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription();
    
}

// All Callbacks, notice how they are declared, this is standard via the FMOD API
FMOD_RESULT F_CALL Plugin_Create(FMOD_DSP_STATE* dsp_state);
FMOD_RESULT F_CALL Plugin_Release(FMOD_DSP_STATE* dsp_state);
FMOD_RESULT F_CALL Plugin_Read(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels);
FMOD_RESULT F_CALL Plugin_Process(FMOD_DSP_STATE* dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY* inbufferarray, FMOD_DSP_BUFFER_ARRAY* outbufferarray, FMOD_BOOL inputsidle, FMOD_DSP_PROCESS_OPERATION op);
FMOD_RESULT F_CALL Plugin_SetBool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL value);
FMOD_RESULT F_CALL Plugin_GetBool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL* value, char* valuestr);
FMOD_RESULT F_CALL Plugin_SetFloats(FMOD_DSP_STATE* dsp_state, int index, float value);

// Our parameters in the plugin
static FMOD_DSP_PARAMETER_DESC mute;    // mutes the audio entirely
static FMOD_DSP_PARAMETER_DESC intensity; //changes our rain intensity
static FMOD_DSP_PARAMETER_DESC volume; // changes the rain's level including farfield rain

//It looks like it's just easier putting your parameter indexes in an enum
enum {
    RAIN_MUTE, //not used, I suspect FMOD orders parameters based on the array below
    RAIN_INTENSITY,
    RAIN_VOLUME,
    RAIN_NUM_PARAMS // this is one way you can count your param # in case you need to add more
};

FMOD_DSP_PARAMETER_DESC* Rain_DSP_Param[3] =
{
    &mute,
    &intensity,
    &volume
};

// This is our plugin template, it follows a strict order, see below
FMOD_DSP_DESCRIPTION Rain_Desc =
{
    FMOD_PLUGIN_SDK_VERSION,    // version
    "Pd Rain",                  // name
    0x00010000,                 // plugin version
    0,                          // # input buffers, for plugin instruments, or generators declare 0, otherwise, use 1-?
    1,                          // # output buffers
    Plugin_Create,              // create (like a constructo)
    Plugin_Release,             // release (like a destructor)
    0,                          // reset
    0,                          // read
    Plugin_Process,             // process
    0,                          // setposition
    RAIN_NUM_PARAMS,            // # parameters
    Rain_DSP_Param,             // pointer to parameter descriptions
    Plugin_SetFloats,           // Set float, where our intensity and volume parameters get handled
    0,                          // Set int
    Plugin_SetBool,             // Set bool
    0,                          // Set data
    0,                          // Get float
    0,                          // Get int
    Plugin_GetBool,             // Get bool
    0,                          // Get data
    0,                          // Check states before processing
    0,                          // User data
    0,                          // System register
    0,                          // System deregister
    0                           // Mixer thread exucute / after execute
};

extern "C"
{
    // This wraps our parameters for the UI
    F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
    {
        FMOD_DSP_INIT_PARAMDESC_BOOL(mute, "On/Off", "", "Turn the rain on or off", false, 0);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(intensity, "Intensity", "", "Set amount of rain.", 0, 100, 50);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(volume, "Volume", "", "Set volume of rain.", 0, 100, 100);
        return &Rain_Desc;
    }
}

/*
We hold our plugin state via a custom class
this is where we would call whatever objects and/or
functions we created with Pd++
see Rain.cpp for how we did that.
*/
class RainState
{
public:
    void SetMute(bool value) { rain_mute = value; }
    bool GetMute() const { return rain_mute; }
    Rain *rain; //we'll put our object on the heap, otherwise it will probably crash
    double playRain() { return rain->perform(); }
    void setRain(float ri, float rv, float cf, float q, float bpv) {
        rain->setRain(ri, rv, cf, q, bpv);
    }

    //Just stealing this from Processing so I don't have to do math
    float map(float input, float rangeLow, float rangeHigh, float targetA, float targetB) {
        // Ensure the input is within bounds
        if (rangeLow == rangeHigh) return targetA; // Avoid division by zero
        // Normalize input to a 0-1 scale based on the source range
        float normalized = (input - rangeLow) / (rangeHigh - rangeLow);
        // Scale to the target range
        return targetA + normalized * (targetB - targetA);
    }

    //These allow us to access our internal parameters in the rain generator
    float rainIntensity;
    float bpCf;
    float bpQ;
    float rainVol;
    float bpRainVol;

private:
    bool rain_mute;
    
};

FMOD_RESULT F_CALL Plugin_Create(FMOD_DSP_STATE* dsp_state)
{
    dsp_state->plugindata = (RainState*)FMOD_DSP_ALLOC(dsp_state, sizeof(RainState));
    RainState* state = (RainState*)dsp_state->plugindata;
    state->rain = new Rain();//You have to create your external objects on the heap

    if (!dsp_state->plugindata)
    {
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALL Plugin_Release(FMOD_DSP_STATE* dsp_state)
{
    RainState* state = (RainState*)dsp_state->plugindata;
    delete state->rain;// don't forget to delete it
    FMOD_DSP_FREE(dsp_state, state);
    return FMOD_OK;
}

//see the FMOD API for the difference between read and process
FMOD_RESULT F_CALL Plugin_Read(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels)
{
    return FMOD_OK;
}

/*
FMOD uses a c-style pointer to our original instantiation of our plugin memory space to pass data back and forth,
see the dsp_state and how it is used
*/

//Because we are a plugin "instrument" or generator, then we can ignore the input buffer. 
FMOD_RESULT F_CALL Plugin_Process(FMOD_DSP_STATE* dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY* /*inbufferarray*/, FMOD_DSP_BUFFER_ARRAY* outbufferarray, FMOD_BOOL /*inputsidle*/, FMOD_DSP_PROCESS_OPERATION op)
{
    
    // This gets call, I think, on our first call to our Process function.
    if (op == FMOD_DSP_PROCESS_QUERY)
    {
        FMOD_SPEAKERMODE outmode = FMOD_SPEAKERMODE_DEFAULT;
        int outchannels = 1; // make sure this matches the # of inputs declared in FMOD_DSP_DESCRIPTION Rain_Desc 

        if (outbufferarray)
        {
            outbufferarray->speakermode = outmode;
            outbufferarray->buffernumchannels[0] = outchannels;
        }
        
    }

    //Otherwise, we are running audio from this condition
    if (op == FMOD_DSP_PROCESS_PERFORM)
    {
        RainState* state = (RainState*)dsp_state->plugindata;
        unsigned int samples = length * outbufferarray[0].buffernumchannels[0];

        while (samples--)
        {
            if (state->GetMute())
            {
                *outbufferarray[0].buffers[0]++ = state->playRain();
            }
            else
            {
                *outbufferarray[0].buffers[0]++ = 0;
            }
        }
    }

    return FMOD_OK;
}

//Mute button
FMOD_RESULT F_CALL Plugin_SetBool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL value)
{
    RainState* state = (RainState*)dsp_state->plugindata;
    state->SetMute(value);
    return FMOD_OK;
}

FMOD_RESULT F_CALL Plugin_GetBool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL* value, char* valuestr)
{
    RainState* state = (RainState*)dsp_state->plugindata;
    *value = state->GetMute();
    return FMOD_OK;
}

//This is how we handle float parameter changes via the GUI thread
FMOD_RESULT F_CALL Plugin_SetFloats(FMOD_DSP_STATE* dsp_state, int index, float value)
{
    RainState* state = (RainState*)dsp_state->plugindata;
   
    switch (index)
    {
    case RAIN_INTENSITY:
        state->rainIntensity = state->map(value, 100, 0, .001, .1);
        state->bpCf = state->map(value, 0, 100, 1400, 900);
        state->bpQ = state->map(value, 0, 100, 2, 3);
        
        break;
    case RAIN_VOLUME:
        state->rainVol = state->map(value, 0, 100, 2, 4);
        state->bpRainVol = state->map(value, 0, 100, 1, 3);
        break;
    }
    state->setRain(state->rainIntensity, state->rainVol, state->bpCf, state->bpQ, state->bpRainVol);
    return FMOD_OK;
}