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
    session = dateTime.toString("ddd_dd_MM_HH_mm");

}

// Charge - volts, battery and ambient temperatures
// Discharge - volts, current, motor, controller, ambient, gps
void LoggerAgent::ProfileLog(QStringList record)
{
    if (record.empty())
    {
        if (charging)
            qDebug() << "Charge profile emtpy record.";
        else
            qDebug() << "Discharge profile emtpy record.";
        return;
    }

    QString logfile;
    if (charging)
        logfile = QString(CHARGE_PROFILE_LOG_FILE).arg(session);
    else
        logfile = QString(DISCHARGE_PROFILE_LOG_FILE).arg(session);


    qDebug() << logfile;
    QFile f(logfile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        qDebug() << "Unable to open file for appending.";
    QTextStream out(&f);
    out << record.at(0);
    out << endl;

    for (int i=1; i<record.count(); i++)
    {
        out << record[i];
        out << endl;
    }


    f.close();
}



