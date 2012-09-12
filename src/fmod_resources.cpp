#include "fmod_resources.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>

static const char *note[120] =
{
    "C 0", "C#0", "D 0", "D#0", "E 0", "F 0", "F#0", "G 0", "G#0", "A 0", "A#0", "B 0",
    "C 1", "C#1", "D 1", "D#1", "E 1", "F 1", "F#1", "G 1", "G#1", "A 1", "A#1", "B 1",
    "C 2", "C#2", "D 2", "D#2", "E 2", "F 2", "F#2", "G 2", "G#2", "A 2", "A#2", "B 2",
    "C 3", "C#3", "D 3", "D#3", "E 3", "F 3", "F#3", "G 3", "G#3", "A 3", "A#3", "B 3",
    "C 4", "C#4", "D 4", "D#4", "E 4", "F 4", "F#4", "G 4", "G#4", "A 4", "A#4", "B 4",
    "C 5", "C#5", "D 5", "D#5", "E 5", "F 5", "F#5", "G 5", "G#5", "A 5", "A#5", "B 5",
    "C 6", "C#6", "D 6", "D#6", "E 6", "F 6", "F#6", "G 6", "G#6", "A 6", "A#6", "B 6",
    "C 7", "C#7", "D 7", "D#7", "E 7", "F 7", "F#7", "G 7", "G#7", "A 7", "A#7", "B 7",
    "C 8", "C#8", "D 8", "D#8", "E 8", "F 8", "F#8", "G 8", "G#8", "A 8", "A#8", "B 8",
    "C 9", "C#9", "D 9", "D#9", "E 9", "F 9", "F#9", "G 9", "G#9", "A 9", "A#9", "B 9"
};

static const float notefreq[120] =
{
      16.35f,   17.32f,   18.35f,   19.45f,    20.60f,    21.83f,    23.12f,    24.50f,    25.96f,    27.50f,    29.14f,    30.87f,
      32.70f,   34.65f,   36.71f,   38.89f,    41.20f,    43.65f,    46.25f,    49.00f,    51.91f,    55.00f,    58.27f,    61.74f,
      65.41f,   69.30f,   73.42f,   77.78f,    82.41f,    87.31f,    92.50f,    98.00f,   103.83f,   110.00f,   116.54f,   123.47f,
     130.81f,  138.59f,  146.83f,  155.56f,   164.81f,   174.61f,   185.00f,   196.00f,   207.65f,   220.00f,   233.08f,   246.94f,
     261.63f,  277.18f,  293.66f,  311.13f,   329.63f,   349.23f,   369.99f,   392.00f,   415.30f,   440.00f,   466.16f,   493.88f,
     523.25f,  554.37f,  587.33f,  622.25f,   659.26f,   698.46f,   739.99f,   783.99f,   830.61f,   880.00f,   932.33f,   987.77f,
    1046.50f, 1108.73f, 1174.66f, 1244.51f,  1318.51f,  1396.91f,  1479.98f,  1567.98f,  1661.22f,  1760.00f,  1864.66f,  1975.53f,
    2093.00f, 2217.46f, 2349.32f, 2489.02f,  2637.02f,  2793.83f,  2959.96f,  3135.96f,  3322.44f,  3520.00f,  3729.31f,  3951.07f,
    4186.01f, 4434.92f, 4698.64f, 4978.03f,  5274.04f,  5587.65f,  5919.91f,  6271.92f,  6644.87f,  7040.00f,  7458.62f,  7902.13f,
    8372.01f, 8869.84f, 9397.27f, 9956.06f, 10548.08f, 11175.30f, 11839.82f, 12543.85f, 13289.75f, 14080.00f, 14917.24f, 15804.26f
};

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

STATUS fmodDetectPitch( FMOD::System* system, FMOD::Channel* channel, Pitch* pitch )
{
    if( system == 0 )
    {
        DEBUG_OUT( "system == NULL" );
        return PARAM_NULL_PASSED;
    }

    if( channel == 0 )
    {
        DEBUG_OUT( "channel == NULL" );
        return PARAM_NULL_PASSED;
    }

    if( note == 0 )
    {
        DEBUG_OUT( "note == NULL" );
        return PARAM_NULL_PASSED;
    }

    if( pitch == 0 )
    {
        DEBUG_OUT( "pitch == NULL" );
        return PARAM_NULL_PASSED;
    }

    //--------------------------------------------------------------------------------------

    static float spectrum[ SPECTRUMSIZE ];
    float        dominantHz = 0;
    float        max;
    float        bin;
    float        binSize = BINSIZE;
    int          dominantNote = 0;

    FMOD_RESULT result;

    //------------------------------------------------

    result = channel->getSpectrum( spectrum, SPECTRUMSIZE, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE );

    if( result != FMOD_OK )
    {
        DEBUG_OUT( FMOD_ErrorString( result ) );
        return CHANNEL_SPECTRUM_READ_FAILED;
    }

    max = 0;
    std::cout << "a" << std::endl;

    for( int i = 0; i < SPECTRUMSIZE; i++ )
    {
        if( spectrum[ i ] > 0.01f && spectrum[ i ] > max )
        {
            max = spectrum[ i ];
            bin = i;
        }
    }
    std::cout << "b" << std::endl;

    dominantHz   = bin * BINSIZE;
    dominantNote = 0;

    for( int i = 0; i < 120; i++ )
    {
        if( dominantHz >= notefreq[ i ] && dominantHz < notefreq[ i + 1 ] )
        {
            // Which is it closer to? This note or the next
            if( fabs( dominantHz - notefreq[ i ] ) < fabs( dominantHz - notefreq[ i + 1 ] ) )
            {
                dominantNote = i;
            }
            else
            {
                dominantNote = i + 1;
            }

            break;
        }
    }
    std::cout << "c\t" << dominantNote << "\t" << dominantHz << std::endl;

    pitch->hz     = dominantHz;
    pitch->noteHz = notefreq[ dominantNote ];
    pitch->note   = note[ dominantNote ];

    std::cout << "d" << std::endl;

    system->update( );
    std::cout << "e" << std::endl;

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
