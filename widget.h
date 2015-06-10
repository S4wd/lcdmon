#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <vector>

#include "busagent.h"
#include "graphicsitems.h"
#include "poweragent.h"
#include "ioagent.h"
#include "logger.h"


#define HIGHER_TEMP(a,b,c)     a = (b > c) ? b:c


const QStringList busAddress = QStringList() << "/dev/ttyS1" << "/dev/ttyS2" << "/dev/ttyS3" << "/dev/ttyS4";





namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();


private slots:

    void                SlotFlashTimerTimeout();
    void                SlotIOBat12VTimerTimeout();
    void                SlotLoggingTimerTimeout();

public slots:

    // temperatures
    void                SlotBus1NewTemps(QStringList newtemps);
    void                SlotBus2NewTemps(QStringList newtemps);
    void                SlotBus3NewTemps(QStringList newtemps);
    void                SlotBus4NewTemps(QStringList newtemps);

    // power
    void                SlotPowerNewReadings(QStringList readings);

    // IO
    void                SlotIOIgnitionState(bool state);
    void                SlotIOBat12VState(bool state);

signals:

    // power
    void                SignalPowerConfigure();

    // IO
    void                SignalIOConfigure();
    void                SignalIOIgnitionState();
    void                SignalIOBat12VState();
    void                SignalIOSirenOn(int alarmEvent);
    void                SignalIOSirenOff(int alarmEvent);

    // temperature
    void                SignalBus1Configure(bool);
    void                SignalBus2Configure(bool);
    void                SignalBus3Configure(bool);
    void                SignalBus4Configure(bool);

    // logging
    void                SignalLoggingNewRecord(QStringList record);



private:
    Ui::Widget          *ui;
    CustomScene         *scene;

    TempDevice          tempDevice[16];
    IODevice            ioDevice;
    PowerDevice         powerDevice;


    BusAgent            Buses[4];
    PowerAgent          Power;
    IOAgent             InputOutput;
    Logger              TempLogger;

    QThread             PowerThread;
    QThread             IOThread;
    QThread             BusThread[4];
    QThread             LoggerThread;

    QTimer              FlashTimer;
    QTimer              LoggingTimer;


    void                GetConfiguration();
    void                DevicesUpdateAlarmStatus();

    void                TempLoggerSetup();
    void                PowerAgentSetup();
    void                IOAgentSetup();
    void                TemperatureSetup();
};





#endif // WIDGET_H
