#include "busagent.h"
#include "qglobal.h"
#include <QTime>
#include <QThread>
#include <QDebug>

#ifdef Q_OS_WIN

#else
extern "C" {
#include "s4wd.h"
}
#endif

BusAgent::BusAgent(QObject *parent)
{   
    this->setParent(parent);
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    TempSampleTimer = NULL;
    BusPortNumber = -1;
}

BusAgent::~BusAgent()
{   
    SlotCallerBusAccess(false);
    delete TempSampleTimer;
}

void BusAgent::BusInitialise(QString Address, QStringList SensorList, int thrno)
{
    ThreadNo = thrno;
    BusAddress = Address;
    Sensors = SensorList;
    SensorCount = SensorList.count();
    qDebug() << BusAddress;
    for (int i=0; i<SensorCount; i++)
	qDebug() << SensorList[i];
}

void BusAgent::SlotCallerBusAccess(bool connect)
{
    qDebug() << QString("Bus Thread: %1 ID:  ").arg(ThreadNo) << QThread::currentThreadId();
    qDebug() << QString("Bus Address: %1").arg(BusAddress);

    if ( connect )
    {
#ifdef Q_OS_WIN
    BusPortNumber = 1;
#else
        if (BusPortNumber < 0)
            BusPortNumber = s4d_OWAcquireBusController( BusAddress.toLatin1().data() );
        qDebug() << QString("Bus port number: %1").arg(BusPortNumber);
#endif
        emit SignalCallerBusInitialised(ThreadNo);
//        if (TempSampleTimer != NULL)
  //      {
            TempSampleTimer = new QTimer;
            QObject::connect(TempSampleTimer, SIGNAL(timeout()), this, SLOT(TempSampleTimerTimeout()));
            TempSampleTimer->start(15000);
    //    }

    }

#ifdef Q_OS_WIN

#else
    else
        s4d_OWReleaseBusController( BusPortNumber );
#endif

}


void BusAgent::TempSampleTimerTimeout()
{
    ReadTemperatures();
}

void BusAgent::ReadTemperatures()
{

    QStringList temps;
    QString tstr;
    QString s;
    qint16 i;

    for (i=0; i<SensorCount; i++)
    {

#ifdef Q_OS_WIN

        tstr = QString("%1").arg(s.number( (float)( qrand() % (35 - 26)+26 ), 'f', 1));
        temps.append(tstr);
#else

        float tempf;
        if (BusPortNumber < 0)
        {
            qDebug() << "BusPortNumber < 0. Acquiring ...";
            BusPortNumber = s4d_OWAcquireBusController( BusAddress.toLatin1().data() );
            return;
        }

        tempf = s4d_OWReadTemperature( BusPortNumber, Sensors[i].toLatin1().data() );
        //qDebug() << QString("Thr: %1 Bus:%2: Sensor:%3 Temp: %4").arg(ThreadNo).arg(BusAddress).arg(Sensors[i].toLatin1().data()).arg(s.number(tempf, 'f', 1));


        tstr = QString("%1").arg(s.number( tempf, 'f', 1 ));
        temps.append(tstr);

#endif
    }
    //qDebug() << "Thread ID New temps ready:  " << QThread::currentThreadId();
    emit SignalCallerBusNewTemps(temps);
}

