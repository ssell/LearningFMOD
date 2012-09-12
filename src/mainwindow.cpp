/**
 * Sound test employing the FMOD library.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fmod_resources.h"

#include <iostream>
#include <QDateTime>
#include <sstream>

//------------------------------------------------------------------------------------------

void MainWindow::setState( FMOD_STATE st )
{
    if( state == st )
        return;

    if( state == IDLE )
    {
        if( st == RECORDING )
        {
            ui->buttonPlayback->setEnabled( false );
            ui->buttonBoth->setEnabled( false );
            ui->buttonStop->setEnabled( true );

            ui->editFilename->setEnabled( false );
            ui->driverSelect->setEnabled( false );

            ui->radioOutputALSA->setEnabled( false );
            ui->radioOutputESD->setEnabled( false );
            ui->radioOutputOSS->setEnabled( false );
            ui->spinRecordLength->setEnabled( false );
        }
        else if( st == PLAYING )
        {
            ui->buttonPlayback->setEnabled( false );
            ui->buttonBoth->setEnabled( false );
            ui->buttonStop->setEnabled( true );

            ui->editFilename->setEnabled( false );
            ui->driverSelect->setEnabled( false );

            ui->radioOutputALSA->setEnabled( false );
            ui->radioOutputESD->setEnabled( false );
            ui->radioOutputOSS->setEnabled( false );
            ui->spinRecordLength->setEnabled( false );
        }
    }

    if( state == RECORDING )
    {
        if( st == IDLE )
        {
            ui->buttonPlayback->setEnabled( true );
            ui->buttonBoth->setEnabled( true );
            ui->buttonStop->setEnabled( false );

            ui->editFilename->setEnabled( true );
            ui->driverSelect->setEnabled( true );

            ui->radioOutputALSA->setEnabled( true );
            ui->radioOutputESD->setEnabled( true );
            ui->radioOutputOSS->setEnabled( true );
            ui->spinRecordLength->setEnabled( true );
        }
    }

    if( state == PLAYING )
    {
        if( st == IDLE )
        {
            ui->buttonPlayback->setEnabled( true );
            ui->buttonBoth->setEnabled( true );
            ui->buttonStop->setEnabled( false );

            ui->editFilename->setEnabled( true );
            ui->driverSelect->setEnabled( true );

            ui->radioOutputALSA->setEnabled( true );
            ui->radioOutputESD->setEnabled( true );
            ui->radioOutputOSS->setEnabled( true );
            ui->spinRecordLength->setEnabled( true );
        }
    }

    state = st;
}

//------------------------------------------------------------------------------------------

void MainWindow::updateInfoPanel( )
{
    if( state == RECORDING )
    {
        unsigned elapsed = time->elapsed( );

        if( elapsed >= ( ui->spinRecordLength->value( ) * 1000 ) )
        {
            ui->labelLength->setText( QString::number( ( ui->spinRecordLength->value( ) * 1000 ) ) );
            ui->buttonStop->click( );
        }
        else
        {
            ui->labelLength->setText( QString::number( time->elapsed( ) ) );

        }
    }
    else if( state == PLAYING )
    {
        unsigned elapsed = time->elapsed( );

        if( elapsed > lastLength )
        {
            setState( IDLE );
            timer->stop( );
            delete [ ] time;
            time = 0;
        }
        else
        {
            ui->playbackProgress->setValue( ( unsigned )( ( ( float )elapsed / ( float )lastLength ) * 100 ) % 100 + 1 );

            Pitch pitch;

            fmodDetectPitch( system, channel, &pitch );
            ui->labelSize->setText( QString::number( pitch.hz ) );
        }
    }
    else
    {
        // Disconnect the timer
        timer->stop( );
    }
}

//------------------------------------------------------------------------------------------

void MainWindow::buttonPlaybackClicked( )
{
    if( state != IDLE )
        return;

    if( sound == 0 )
    {
        status = fmodSetup( &system );

        if( status != OK )
        {
            std::cout << "ERROR: fmodSetup failed! [" << status << "]" << std::endl;
        }

        status = fmodSystemInit( system );

        if( status != OK )
        {
            std::cout << "ERROR: fmodSystemInit failed! [" << status << "]" << std::endl;
        }

        QString path = ui->editFilename->text( ) + ".wav";
        status = fmodCreateSoundFromFile( system, &sound, path.toLocal8Bit( ).data( ) );

        if( status != OK )
        {
            std::cout << "fmodCreateSoundFromFile failed! [" << status << "]" << std::endl;
            return;
        }
    }

    system->playSound( FMOD_CHANNEL_REUSE, sound, false, &channel );

    time = new QTime( );
    time->start( );

    timer->start( 76 );
    setState( PLAYING );
}

//------------------------------------------------------------------------------------------

void MainWindow::buttonRecordClicked( )
{
    FMOD_RESULT fmod_result;

    unsigned driver = ui->driverSelect->currentIndex( );

    //------------------------------------------------
    // Specify the output type

    OUTPUT_TYPE output;

    if( ui->radioOutputALSA->isChecked( ) )
        output = ALSA;
    else if( ui->radioOutputESD->isChecked( ) )
        output = ESD;
    else
        output = OSS;

    //------------------------------------------------

    status = fmodSetup( &system );

    if( status != OK )
    {
        std::cout << "ERROR: fmodSetup failed! [" << status << "]" << std::endl;
    }

    status = fmodSystemInit( system );

    if( status != OK )
    {
        std::cout << "ERROR: fmodSystemInit failed! [" << status << "]" << std::endl;
    }

    if( sound != 0 )
        sound->release( );

    status = fmodCreateSound( system, &sound, ui->spinRecordLength->value( ) );

    if( status != OK )
    {
        std::cout << "ERROR: fmodCreateSound failed! [" << status << "]" << std::endl;
    }

    //------------------------------------------------
    // Start recording and updating the info panel

    fmod_result = system->recordStart( driver, sound, false );

    if( fmod_result != FMOD_OK )
    {
        std::cout << "ERROR: recordStart failed! [" << fmod_result << "]" << std::endl;
        std::cout << "> " << FMOD_ErrorString( fmod_result ) << std::endl;
    }

    time = new QTime( );
    time->start( );

    timer->start( 76 );
    setState( RECORDING );
}

//------------------------------------------------------------------------------------------

void MainWindow::buttonStopClicked( )
{
    FMOD_RESULT fmod_result;

    if( state == RECORDING )
    {
        fmod_result = system->recordStop( ui->driverSelect->currentIndex( ) );

        if( fmod_result != FMOD_OK )
        {
            std::cout << "ERROR: recordStop failed! [" << fmod_result << "]" << std::endl;
            std::cout << "> " << FMOD_ErrorString( fmod_result ) << std::endl;
        }

        lastLength = ( time->elapsed( ) > ( ui->spinRecordLength->value( ) * 1000 ) ? ( ui->spinRecordLength->value( ) * 1000 ) : time->elapsed( ) );
        ui->labelLength->setText( QString::number( lastLength ) );

        delete [ ] time;
        time = 0;
    }
    else if( state == PLAYING )
    {
        channel->stop( );
    }

    setState( IDLE );
}

//------------------------------------------------------------------------------------------

void MainWindow::buttonWriteClicked( )
{
    if( sound != 0 )
    {
        std::stringstream filename;
        filename << ui->editFilename->text( ).toLocal8Bit( ).data( ) << ".wav";
        SaveToWav( sound, filename.str( ).c_str( ) );
    }
}

//------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    system      = 0;
    sound       = 0;
    channel     = 0;
    lastLength  = 0;
    timer       = new QTimer( this );
    status      = OK;
    state       = IDLE;

    FMOD::System* tempSystem = 0;

    //------------------------------------------------

    ui->setupUi(this);

    //------------------------------------------------

    status = fmodSetup( &tempSystem );

    if( status != OK )
    {
        std::cout << "ERROR: fmodSetup failed! [" << status << "]" << std::endl;
    }

    status = fmodSystemInit( tempSystem );

    if( status != OK )
    {
        std::cout << "ERROR: fmodSystemInit failed! [" << status << "]" << std::endl;
    }

    //------------------------------------------------

    std::vector< std::string > drivers = getDrivers( tempSystem, &status, true );

    if( status != OK )
    {
        std::cout << "ERROR: getDrivers failed! [" << status << "]" << std::endl;
    }

    for( unsigned i = 0; i < drivers.size( ); i++ )
    {
        ui->driverSelect->addItem( QString( drivers[ i ].c_str( ) ) );
    }

    drivers.clear( );
    drivers = getDrivers( tempSystem, &status, false );

    for( unsigned i = 0; i < drivers.size( ); i++ )
    {
        ui->driverSelectPlayback->addItem( QString( drivers[ i ].c_str( ) ) );
    }

    tempSystem->release( );
    tempSystem = 0;

    //------------------------------------------------

    connect( ui->buttonRecord, SIGNAL( clicked( ) ), this, SLOT( buttonRecordClicked( ) ) );
    connect( ui->buttonStop, SIGNAL( clicked( ) ), this, SLOT( buttonStopClicked( ) ) );
    connect( ui->buttonPlayback, SIGNAL( clicked( ) ), this, SLOT( buttonPlaybackClicked( ) ) );
    connect( ui->buttonWrite, SIGNAL( clicked( ) ), this, SLOT( buttonWriteClicked( ) ) );
    connect( timer, SIGNAL( timeout( ) ), this, SLOT( updateInfoPanel( ) ) );
}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;

    sound->release( );
    system->release( );
}
