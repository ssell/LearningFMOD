#include "fmod_resources.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

//------------------------------------------------------------------------------------------

void DEBUG_OUT( const char* str )
{
    std::cout << "ERROR: " << str << std::endl;
}

//------------------------------------------------------------------------------------------

/**
 * Initializes the provided FMOD::System
 */
STATUS fmodSetup( FMOD::System** system )
{
    FMOD_RESULT result;

    //------------------------------------------------
    // Create the FMOD System

    result = FMOD::System_Create( system );

    if( result != FMOD_OK || system == 0 )
    {
        if( result != FMOD_OK )
            DEBUG_OUT( FMOD_ErrorString( result ) );
        else
            DEBUG_OUT( "System creation failed!" );

        return SYSTEM_CREATION_FAILED;
    }

    return OK;
}

STATUS fmodSystemInit( FMOD::System* system )
{
    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    FMOD_RESULT result;
    unsigned version;

    result = system->getVersion( &version );

    if( result != FMOD_OK || version < FMOD_VERSION )
    {
        if( result != FMOD_OK )
            DEBUG_OUT( FMOD_ErrorString( result ) );
        else
            DEBUG_OUT( "FMOD version too low!" );

        return SYSTEM_VERSION_CHECK_FAILED;
    }

    //------------------------------------------------
    // Initialize the System

    result = system->init( MAX_NUM_CHANNELS, FMOD_INIT_NORMAL, 0 );

    if( result != FMOD_OK )
    {
        DEBUG_OUT( FMOD_ErrorString( result ) );
        return SYSTEM_INITIALIZATION_FAILED;
    }

    return OK;
}

//------------------------------------------------------------------------------------------

STATUS fmodCreateSound( FMOD::System* system, FMOD::Sound** sound, unsigned max_length )
{
    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    FMOD_RESULT result;

    FMOD_CREATESOUNDEXINFO exInfo;
    memset( &exInfo, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );

    exInfo.cbsize           = sizeof( FMOD_CREATESOUNDEXINFO );
    exInfo.numchannels      = 3;
    exInfo.format           = FMOD_SOUND_FORMAT_PCM16;
    exInfo.defaultfrequency = 44100;
    exInfo.length           = exInfo.defaultfrequency * sizeof( short ) * exInfo.numchannels * max_length;

    result = system->createSound( 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exInfo, sound );

    if( result != FMOD_OK || sound == 0 )
    {
        if( result != FMOD_OK )
            DEBUG_OUT( FMOD_ErrorString( result ) );
        else
            DEBUG_OUT( "Sound object creation failed!" );

        return SOUND_CREATION_FAILED;
    }

    return OK;
}

//------------------------------------------------------------------------------------------

STATUS fmodCreateSoundFromFile( FMOD::System* system, FMOD::Sound** sound, const char* file )
{
    FMOD_RESULT result;
    FMOD_CREATESOUNDEXINFO exInfo;

    void* buff;
    int   length;

    //------------------------------------------------

    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    if( file == 0 )
    {
        DEBUG_OUT( "file == NULL" );
        return PARAM_NULL_PASSED;
    }

    if( !LoadFileIntoMemory( file, &buff, &length ) )
    {
        DEBUG_OUT( "LoadFileIntoMemory failed!" );
        DEBUG_OUT( file );
        return SOUND_FROM_FILE_FAILED;
    }

    memset( &exInfo, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );
    exInfo.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );
    exInfo.length = length;

    result = system->createSound( ( const char* )buff, FMOD_HARDWARE | FMOD_OPENMEMORY, &exInfo, sound );

    if( result != FMOD_OK || sound == 0 )
    {
        if( result != FMOD_OK )
            DEBUG_OUT( FMOD_ErrorString( result ) );
        else
            DEBUG_OUT( "Sound object creation failed!" );

        free( buff );
        return SOUND_CREATION_FAILED;
    }

    free( buff );
    return OK;
}

//------------------------------------------------------------------------------------------

/**
 * Prepares the FMOD::System for recording by setting output type and drivers, and creating the Sound object.
 */
STATUS fmodSetOutputType( FMOD::System* system, OUTPUT_TYPE output )
{
    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    FMOD_RESULT result;

    //------------------------------------------------
    // Set Output type

    switch( output )
    {
    case ALSA:
        result = system->setOutput( FMOD_OUTPUTTYPE_ALSA );
        break;
    case ESD:
        result = system->setOutput( FMOD_OUTPUTTYPE_ESD );
        break;
    default:
        result = system->setOutput( FMOD_OUTPUTTYPE_OSS );
        break;
    }

    if( result != FMOD_OK )
    {
        DEBUG_OUT( FMOD_ErrorString( result ) );
        return OUTPUT_TYPE_SET_FAILED;
    }

    return OK;
}

//------------------------------------------------------------------------------------------

STATUS fmodSetPlaybackDriver( FMOD::System* system, unsigned playback_driver )
{
    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    FMOD_RESULT result;

    result = system->setDriver( playback_driver );

    if( result != FMOD_OK )
    {
        DEBUG_OUT( FMOD_ErrorString( result ) );
        return DRIVER_PLAYBACK_SET_FAILED;
    }

    return OK;
}

//------------------------------------------------------------------------------------------

std::vector< std::string > getDrivers( FMOD::System* system, STATUS* error, bool record_drivers )
{
    FMOD_RESULT result;
    int num_drivers;

    std::vector< std::string > drivers;

    *error = OK;

    //------------------------------------------------

    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        *error = PARAM_NULL_PASSED;
        return drivers;
    }

    result = ( record_drivers ? system->getRecordNumDrivers( &num_drivers ) : system->getNumDrivers( &num_drivers ) );

    if( result != FMOD_OK )
    {
        DEBUG_OUT( FMOD_ErrorString( result ) );
        *error = DRIVER_COUNT_RETRIEVE_FAILED;
        return drivers;
    }

    for( int i = 0; i < num_drivers; i++ )
    {
        char info[ 256 ];

        result = ( record_drivers ? system->getRecordDriverInfo( i, info, 256, 0 ) : system->getDriverInfo( i, info, 256, 0 ) );

        if( result != FMOD_OK )
        {
            *error = DRIVER_FETCH_FAILED;
            return drivers;
        }

        drivers.push_back( info );
    }

    return drivers;
}

//------------------------------------------------------------------------------------------

bool LoadFileIntoMemory( const char *name, void **buff, int *length )
{
    FILE *fp = fopen(name, "rb");

    if( fp == 0 )
        return false;

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buff = malloc(*length);
    fread(*buff, *length, 1, fp);

    fclose(fp);

    return true;
}

//------------------------------------------------------------------------------------------

void SaveToWav(FMOD::Sound *sound, const char* file_name )
{
    FILE *fp;
    int             channels, bits;
    float           rate;
    void           *ptr1, *ptr2;
    unsigned int    lenbytes, len1, len2;

    if (!sound)
    {
        return;
    }

    sound->getFormat  (0, 0, &channels, &bits);
    sound->getDefaults(&rate, 0, 0, 0);
    sound->getLength  (&lenbytes, FMOD_TIMEUNIT_PCMBYTES);

    {
        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack(1)
        #endif

        /*
            WAV Structures
        */
        typedef struct
        {
            signed char id[4];
            int 		size;
        } RiffChunk;

        struct
        {
            RiffChunk       chunk           __PACKED;
            unsigned short	wFormatTag      __PACKED;    /* format type  */
            unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
            unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
            unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
            unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
            unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
        } __PACKED FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

        struct
        {
            RiffChunk   chunk;
        } DataChunk = { {{'d','a','t','a'}, lenbytes } };

        struct
        {
            RiffChunk   chunk;
            signed char rifftype[4];
        } WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + lenbytes }, {'W','A','V','E'} };

        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack()
        #endif

        fp = fopen( file_name, "wb");

        /*
            Write out the WAV header.
        */
        fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
        fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
        fwrite(&DataChunk, sizeof(DataChunk), 1, fp);

        /*
            Lock the sound to get access to the raw data.
        */
        sound->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);

        /*
            Write it to disk.
        */
        fwrite(ptr1, len1, 1, fp);

        /*
            Unlock the sound to allow FMOD to use it again.
        */
        sound->unlock(ptr1, ptr2, len1, len2);

        fclose(fp);
    }
}
