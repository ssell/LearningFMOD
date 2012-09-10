#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//------------------------------------------------------------------------------------------

#include <QMainWindow>
#include <QTimer>

#include "fmod_resources.h"

//------------------------------------------------------------------------------------------

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    void setState( FMOD_STATE st );

private slots:

    void buttonRecordClicked( );
    void buttonStopClicked( );
    void buttonPlaybackClicked( );
    void updateInfoPanel( );
    
private:
    Ui::MainWindow *ui;
    QTimer* timer;
    QTime*  time;

    FMOD::System*  system;
    FMOD::Sound*   sound;
    FMOD::Channel* channel;

    STATUS     status;
    FMOD_STATE state;

    unsigned lastLength;
};

//------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
