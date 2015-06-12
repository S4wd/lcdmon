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
    ReadGpsTimer->setInterval(8000);
    connect(ReadGpsTimer, SIGNAL(timeout()),this,SLOT(ReadGpsTimerTimeout()));
    ReadGpsTimer->start();
}



void GpsAgent::ReadGpsTimerTimeout()
{
    ReadGpsTimer->stop();
    QStringList data;

#ifdef Q_OS_WIN

    // long, lat, speed, altitude, heading (magnetic north)
    data << "37" << "S" << "140" << "E" << "10.3" << "31.6M" << "178";
    emit SignalNewData(data);

#else


    float lat;
    QString latC;
    float lon;
    QString lonC;
    float heading;
    float speed;
    float altitude;

    bool GGADone;
    bool VTGDone;



    QFile devtty(QString("/dev/ttyS1"));

    if (!devtty.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Could not open /dev/ttyS1 for reading.";
        return;
    }

    //qDebug() << "Opened /dev/ttyS1 for reading.";

    GGADone = false;
    VTGDone = false;


    QStringList tokens;
    QString line;


    for (int i=0;i<20;i++)
    {
       QByteArray ba = devtty.readLine(800);
       line = QString(ba);
       tokens = line.split(",");
//       qDebug() << tokens;

       // heading and speed
       if (tokens[0] == QString("$GPVTG") && !VTGDone)
       {
           // [1] heading - true north. [3] heading - magentic north
           heading = tokens[1].toFloat();
	   // [5] speed - knots [7] speed in km/h
           speed = tokens[7].toFloat();
           VTGDone = true;
       }
       else if (tokens[0] == QString("$GPGGA") && !GGADone)
       {
           lon = tokens[2].toFloat() / 100;
           lonC = tokens[3];
           lat = tokens[4].toFloat() / 100;
           latC = tokens[5];
           altitude = tokens[9].toFloat();
           GGADone = true;
       }
//       else
  //         qDebug() << line;

       if (GGADone && VTGDone)
           break;

    }

    devtty.close();

    data.clear();

    data << QString("%1").arg(lon);
    data << lonC;
    data << QString("%1").arg(lat);
    data << latC;
    data << QString("%1").arg(speed);
    data << QString("%1").arg(heading);
    data << QString("%1").arg(altitude);

    //qDebug() << data;

    emit SignalNewData(data);



#endif

    ReadGpsTimer->start();
}



