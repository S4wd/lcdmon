#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>

#ifdef Q_OS_WIN
#define TEMPERATURE_LOGGING_FILE               "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\debug\\Temp\\%1temp"
#define TEMPERATURE_META_FILE                  "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\debug\\Temp\\meta"
#else
#define TEMPERATURE_LOGGING_FILE               "/home/s4wd/logging/temperature/%1temp"
#define TEMPERATURE_META_FILE                  "/home/s4wd/logging/temperature/meta"
#endif

class Logger : public QObject
{
    Q_OBJECT

public:
    Logger(QObject *parent = 0);
    ~Logger();

public slots:
    void SlotNewRecord(QStringList sample);

private:
    int samplesCaptured;
    int filePrefix;
    void getParams();
    void setParams();
    void logToFile(QStringList record);

};

#endif // LOGGER_H
