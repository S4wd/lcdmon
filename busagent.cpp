#include "busagent.h"
#include "qglobal.h"
#include <QTime>
#include <QThread>
#include <QDebug>


extern "C" {
#include "s4wd.h"
}


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
        if (BusPortNumber < 0)
            BusPortNumber = s4d_OWAcquireBusController( BusAddress.toLatin1().data() );
        qDebug() << QString("Bus port number: %1").arg(BusPortNumber);

        if (TempSampleTimer != NULL)
        {
            TempSampleTimer = new QTimer;
            QObject::connect(TempSampleTimer, SIGNAL(timeout()), this, SLOT(TempSampleTimerTimeout()));
            TempSampleTimer->start(15000);
        }

    }
    else
        s4d_OWReleaseBusController( BusPortNumber );


}


void BusAgent::TempSampleTimerTimeout()
{
    ReadTemperatures();
}

void BusAgent::ReadTemperatures()
{
    qint16 i;
    QStringList temps;
    QString tstr;
    QString s;
    float tempf;

    if (BusPortNumber < 0)
    {
    qDebug() << "BusPortNumber < 0. Acquiring ...";
    BusPortNumber = s4d_OWAcquireBusController( BusAddress.toLatin1().data() );
	return;
    }

    for (i=0; i<SensorCount; i++)
    {
        tempf = s4d_OWReadTemperature( BusPortNumber, Sensors[i].toLatin1().data() );
	//qDebug() << QString("Thr: %1 Bus:%2: Sensor:%3 Temp: %4").arg(ThreadNo).arg(BusAddress).arg(Sensors[i].toLatin1().data()).arg(s.number(tempf, 'f', 1));

	//if ( tempf == 200.0 )
        //	tstr = QString("%1").arg(s.number( (float)( qrand() % (35 - 26)+26 ), 'f', 1));
        //else
	tstr = QString("%1").arg(s.number( tempf, 'f', 1 ));
	temps.append(tstr);
    }


    //qDebug() << "Thread ID New temps ready:  " << QThread::currentThreadId();
    emit SignalCallerBusNewTemps(temps);
}

