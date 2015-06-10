#include "poweragent.h"

PowerAgent::PowerAgent(QObject *parent)
{
    parent = 0;
}

PowerAgent::~PowerAgent()
{
    delete ADSTimer;
    delete LogTimer;
    delete ADSProcess;
}

void PowerAgent::SlotConfigure()
{
    getMeta();
    loggingToFile = false;

    qDebug() << " Thread ID:  " << QThread::currentThreadId();

    /*QTime time = QTime::currentTime();
    qsrand((uint)time.msec());*/

    // ADS process
    ADSProcess = new QProcess(this);
    QObject::connect(ADSProcess, SIGNAL(finished(int)), this, SLOT(SlotADSFinished(int)));

    // ADS timer
    ADSTimer = new QTimer(this);
    QObject::connect(ADSTimer, SIGNAL(timeout()),this,SLOT(ADSTimerTimeout()));
    ADSTimer->start(2000);

    // Logging timer
    LogTimer = new QTimer(this);
    QObject::connect(LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimeout()));
    LogTimer->start(303000);
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
    if (newdata.voltage < 40)
    newdata.voltage = 40;

    if (newdata.voltage > 52)
    newdata.voltage = 52;



    QStringList readings;
    QString s;

    readings << QString("%1").arg(s.number(newdata.current,'f',1))
             << QString("%1").arg(s.number(newdata.voltage,'f',1));

    emit SignalNewReading(readings);



    // process data
    ProcessNewData();
}







void PowerAgent::LogToFile()
{


    QFile f(QString(POWER_LOG_FILE).arg(filePrefix));
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);


    DataRecord record;
    QString s;

    qint16 totalSamples = myData.count();

    for (int i=0; i<totalSamples; i++)
    {
        record = myData[i];
        out << QString("%1,").arg(record.datetime);
        out << QString("%1,").arg(s.number(record.voltage, 'f', 1));
        out << QString("%1,").arg(s.number(record.current, 'f', 0));
        out << QString("%1,").arg(s.number(record.velocity, 'f', 1));
        out << QString("%1").arg(s.number(record.altitude, 'f', 1));
        out << endl;
    }
    f.close();



    samplesCaptured += totalSamples;
    qDebug() << QString("Power Samples captured: %1").arg(samplesCaptured);

    if (samplesCaptured >= 1000)
    {
        samplesCaptured = 0;
        filePrefix += 1;
        setMeta();

        //qDebug() << QString("Prefix: %1").arg(filePrefix);
    }
    else
        setMeta();

    // clear buffered data
    myData.clear();
}

void PowerAgent::ADSTimerTimeout()
{
    if(!loggingToFile)
        ADSRead();
}

void PowerAgent::LogTimerTimeout()
{
    // stop the ADS timer
    ADSTimer->stop();

    loggingToFile = true;
    LogToFile();
    loggingToFile = false;

    ADSTimer->start();

}

void PowerAgent::ProcessNewData()
{
    QDateTime dateTime = dateTime.currentDateTime();
    newdata.datetime = dateTime.toString("yy-MM-dd hh:mm:ss");
    newdata.velocity = 0;
    newdata.altitude = 0;
    myData.push_back(newdata);
}

void PowerAgent::ADSRead()
{
    QString adsapp = "/usr/src/qt/s4wd_test/ADS1115";
    QStringList argslist = QStringList() << "get";
    //qDebug() << "Start Bankvoltage reading.";
    ADSProcess->start(adsapp,argslist);
}



void PowerAgent::getMeta()
{
    QSettings settings(QString(POWER_META_FILE), QSettings::IniFormat);

    settings.beginGroup(QString("logger"));
    filePrefix = settings.value(QString("prefix")).toInt();
    samplesCaptured = settings.value(QString("sample")).toInt();


    qDebug() << QString("Get Power Prefix: %1").arg(filePrefix);
    qDebug() << QString("Get Power Samples: %1").arg(samplesCaptured);
    settings.endGroup();

}

void PowerAgent::setMeta()
{
    QSettings settings(QString(POWER_META_FILE), QSettings::IniFormat);

    settings.beginGroup(QString("logger"));
    settings.setValue( QString("prefix"), QVariant(filePrefix).toString() );
    settings.setValue( QString("sample"), QVariant(samplesCaptured).toString() );

    qDebug() << QString("Set Power Prefix: %1").arg(filePrefix);
    qDebug() << QString("Set Power Samples: %1").arg(samplesCaptured);
    settings.endGroup();
}



