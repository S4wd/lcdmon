#include "logger.h"

Logger::Logger(QObject *parent)
{
    parent = 0;
    getParams();

}

Logger::~Logger()
{
}


void Logger::SlotNewRecord(QStringList sample)
{
    logToFile(sample);
}

void Logger::getParams()
{
    QSettings settings(QString(TEMPERATURE_META_FILE), QSettings::IniFormat);

    settings.beginGroup(QString("logger"));
    filePrefix = settings.value(QString("prefix")).toInt();
    samplesCaptured = settings.value(QString("sample")).toInt();


    qDebug() << QString("Get Temp Prefix: %1").arg(filePrefix);
    qDebug() << QString("Get Temp Samples: %1").arg(samplesCaptured);
    settings.endGroup();

}

void Logger::setParams()
{
    QSettings settings(QString(TEMPERATURE_META_FILE), QSettings::IniFormat);

    settings.beginGroup(QString("logger"));
    settings.setValue( QString("prefix"), QVariant(filePrefix).toString() );
    settings.setValue( QString("sample"), QVariant(samplesCaptured).toString() );

    qDebug() << QString("Set Temp Prefix: %1").arg(filePrefix);
    qDebug() << QString("Set Temp Samples: %1").arg(samplesCaptured);
    settings.endGroup();
}

void Logger::logToFile(QStringList record)
{


    if (samplesCaptured == 1000)
    {
        samplesCaptured = 0;
        filePrefix += 1;
        setParams();

        //qDebug() << QString("Temp Prefix: %1").arg(filePrefix);
    }


    QFile f(QString(TEMPERATURE_LOGGING_FILE).arg(filePrefix));
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    QDateTime dateTime = dateTime.currentDateTime();

    QString dateTimeString = dateTime.toString("yy-MM-dd hh:mm:ss");
    out << dateTimeString;

    for (int i=0; i<record.count(); i++)
        out << QString(",%1").arg(record.at(i).toLatin1().data());

    out << endl;
    f.close();

    samplesCaptured++;
    qDebug() << QString("Temp Samples captured: %1").arg(samplesCaptured);
    setParams();
}

