#ifndef FMOD_RESOURCES_H
#define FMOD_RESOURCES_H

#include "fmod.hpp"
#include "fmod_errors.h"

#include <string>
#include <vector>
#include <iostream>

//------------------------------------------------------------------------------------------

#define MAX_NUM_CHANNELS      1

//------------------------------------------------------------------------------------------

enum STATUS
{
    OK = 0,
    PARAM_NULL_PASSED,
    SYSTEM_CREATION_FAILED,
    SYSTEM_INITIALIZATION_FAILED,
    SYSTEM_VERSION_CHECK_FAILED,
    OUTPUT_TYPE_SET_FAILED,
    DRIVER_PLAYBACK_SET_FAILED,
    DRIVER_COUNT_RETRIEVE_FAILED,
    DRIVER_FETCH_FAILED,
    SOUND_CREATION_FAILED
};

enum OUTPUT_TYPE
{
    OSS = 0,
    ALSA,
    ESD
};

enum FMOD_STATE
{
    IDLE = 0,
    RECORDING,
    PLAYING
};

void   DEBUG_OUT( const char* );
STATUS fmodSetup( FMOD::System** system );
STATUS fmodSystemInit( FMOD::System* system );
STATUS fmodCreateSound( FMOD::System* system, FMOD::Sound** sound, unsigned max_length );
STATUS fmodSetOutputType( FMOD::System* system, OUTPUT_TYPE output = OSS );
STATUS fmodSetPlaybackDriver( FMOD::System* system, unsigned playback_driver );

std::vector< std::string > getDrivers( FMOD::System* system, STATUS* error, bool record_drivers = true );

//------------------------------------------------------------------------------------------

#endif // FMOD_RESOURCES_H
