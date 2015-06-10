#include "logger.h"

LoggerAgent::LoggerAgent(QObject *parent)
{
    parent = 0;
}

LoggerAgent::~LoggerAgent()
{
}


void LoggerAgent::SlotNewRecord(QStringList sample)
{
    ProfileLog(sample);
}

void LoggerAgent::SlotStartNewLogSession(bool charge)
{
    charging = charge;

    QDateTime dateTime = dateTime.currentDateTime();
    session = dateTime.toString("ddd_dd_HH_mm");

    QDir dir;
    if (charge)
        dir.setPath(QString(CHARGE_PROFILE_LOG_DIR));
    else
        dir.setPath(QString(DISCHARGE_PROFILE_LOG_DIR));

    if (dir.mkdir(session))
        qDebug() << QString("New session: %1").arg(session);
}

// Charge - volts, battery and ambient temperatures
// Discharge - volts, current, motor, controller, ambient, gps
void LoggerAgent::ProfileLog(QStringList record)
{
    if (record.empty())
    {
        if (charging)
            qDebug() << "Ccharge profile emtpy record.";
        else
            qDebug() << "Discharge profile emtpy record.";
        return;
    }

    QString logfile;
    if (charging)
        logfile = QString(CHARGE_PROFILE_LOG_FILE).arg(session);
    else
        logfile = QString(DISCHARGE_PROFILE_LOG_FILE).arg(session);

    QFile f(logfile);
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    out << record.at(0);

    for (int i=1; i<record.count(); i++)
    {
        out << record[i];
        out << endl;
    }


    f.close();
}



