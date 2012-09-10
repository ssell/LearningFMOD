#include "fmod_resources.h"
#include <iostream>
#include <cstring>

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
