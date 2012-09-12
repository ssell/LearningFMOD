#ifndef FMOD_RESOURCES_H
#define FMOD_RESOURCES_H

#include "fmod.hpp"
#include "fmod_errors.h"

#include <string>
#include <vector>
#include <iostream>

//------------------------------------------------------------------------------------------

#if defined(WIN32) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
    #define __PACKED                         /* dummy */
#else
    #define __PACKED __attribute__((packed)) /* gcc packed */
#endif

#define MAX_NUM_CHANNELS  1
#define OUTPUTRATE        48000
#define SPECTRUMSIZE      8192
#define SPECTRUMRANGE     ((float)OUTPUTRATE / 2.0f)      /* 0 to nyquist */
#define BINSIZE           (SPECTRUMRANGE / (float)SPECTRUMSIZE)

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
    SOUND_CREATION_FAILED,
    SOUND_FROM_FILE_FAILED,
    CHANNEL_SPECTRUM_READ_FAILED
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

struct Pitch
{
    float hz;
    float noteHz;
    const char* note;
};

void   DEBUG_OUT( const char* );
STATUS fmodSetup( FMOD::System** system );
STATUS fmodSystemInit( FMOD::System* system );
STATUS fmodCreateSound( FMOD::System* system, FMOD::Sound** sound, unsigned max_length );
STATUS fmodCreateSoundFromFile( FMOD::System* system, FMOD::Sound** sound, const char* file );
STATUS fmodSetOutputType( FMOD::System* system, OUTPUT_TYPE output = OSS );
STATUS fmodSetPlaybackDriver( FMOD::System* system, unsigned playback_driver );
STATUS fmodDetectPitch( FMOD::System* system, FMOD::Channel* channel, Pitch* pitch );

void SaveToWav(FMOD::Sound *sound, const char* file_name );
bool LoadFileIntoMemory( const char *name, void **buff, int *length );

std::vector< std::string > getDrivers( FMOD::System* system, STATUS* error, bool record_drivers = true );

//------------------------------------------------------------------------------------------

#endif // FMOD_RESOURCES_H
