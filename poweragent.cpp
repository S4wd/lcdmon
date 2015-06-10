#include "poweragent.h"

PowerAgent::PowerAgent(QObject *parent)
{
    parent = 0;
}

PowerAgent::~PowerAgent()
{
    delete ADSTimer;
    delete ADSProcess;
}

void PowerAgent::SlotConfigure()
{
    qDebug() << "Power Thread ID:  " << QThread::currentThreadId();

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    // ADS process
    ADSProcess = new QProcess(this);
    QObject::connect(ADSProcess, SIGNAL(finished(int)), this, SLOT(SlotADSFinished(int)));

    // ADS timer
    ADSTimer = new QTimer(this);
    QObject::connect(ADSTimer, SIGNAL(timeout()),this,SLOT(ADSTimerTimeout()));
    ADSTimer->start(2000);
}




void PowerAgent::SlotADSFinished(int exitstatus)
{
    Q_UNUSED(exitstatus);

    QByteArray data = ADSProcess->readAllStandardOutput();

    ADSProcess->close();
    QList<QByteArray> tokens = data.split(',');

    bool ok;
    newdata.current = tokens[1].toFloat(&ok);
    if (newdata.current < 0)
        newdata.current = 0;

    if (newdata.current > 400)
        newdata.current = 400;



    newdata.voltage = tokens[0].toFloat(&ok);
    if (newdata.voltage < 42)
    newdata.voltage = 42;

    if (newdata.voltage > 60)
    newdata.voltage = 60;



    QStringList readings;
    QString s;

    readings << QString("%1").arg(s.number(newdata.current,'f',1))
             << QString("%1").arg(s.number(newdata.voltage,'f',1));

    emit SignalNewReading(readings);
}


void PowerAgent::ADSTimerTimeout()
{
    ADSRead();
}

void PowerAgent::ADSRead()
{
#ifdef Q_OS_WIN
    QStringList readings;
    QString s;
    newdata.current = (float)( qrand() % (180 - 150)+150 );
    newdata.voltage = (float)( qrand() % (50 - 48)+48 );
    readings << QString("%1").arg(s.number(newdata.current,'f',1))
             << QString("%1").arg(s.number(newdata.voltage,'f',1));

    emit SignalNewReading(readings);
#else

    QString adsapp = "/usr/src/qt/s4wd_test/ADS1115";
    QStringList argslist = QStringList() << "get";
    //qDebug() << "Start Bankvoltage reading.";
    ADSProcess->start(adsapp,argslist);
#endif
}




