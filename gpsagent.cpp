#include "gpsagent.h"

GpsAgent::GpsAgent(QObject *parent) : QObject(parent)
{
    parent = 0;
}

GpsAgent::~GpsAgent()
{
    qDebug() << "Destoying GPS agent";
    delete ReadGpsTimer;
}

void GpsAgent::SlotInitialise()
{
    qDebug() << "Gps Thread ID:  " << QThread::currentThreadId();
    ReadGpsTimer = new QTimer;
    connect(ReadGpsTimer, SIGNAL(timeout()),this,SLOT(ReadGpsTimerTimeout()));
    connect(&devTTyS, SIGNAL(finished(int,QProcess::ExitStatus)), this,SLOT(SlotProcessFinished(int,QProcess::ExitStatus)));
    ReadGpsTimer->start(5000);
}



void GpsAgent::ReadGpsTimerTimeout()
{

#ifdef Q_OS_WIN
    QStringList data;
    // long, lat, speed, altitude, heading (magnetic north)
    data << "4003.9039N" << "10512.5793W" << "10.3" << "230.3" << "31.6M";
    emit SignalNewData(data);

#else

    if (devTTyS.state() != QProcess::NotRunning)
    {
        devTTyS.close();
        return;
    }

    QString tail = "tail";
    QStringList argslist = QStringList() << "-n" << "20" << "/dev/ttyS1";
    devTTyS.start(tail,argslist);
#endif
}

void GpsAgent::SlotProcessFinished(int,QProcess::ExitStatus)
{
    QString data = QString(devTTyS.readAll());

    // separate into lines and parse and pull the various NMEA data

}

