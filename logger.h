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
#include <QDir>

#define VAUGHN_HOME

#ifdef Q_OS_WIN

    #ifdef VAUGHN_HOME
        #define CHARGE_PROFILE_LOG_DIR                "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Charge\\"
        #define DISCHARGE_PROFILE_LOG_DIR             "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Discharge\\"
        #define CHARGE_PROFILE_LOG_FILE               "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Charge\\%1\\dat.log"
        #define DISCHARGE_PROFILE_LOG_FILE            "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Discharge\\%1\\dat.log"
        #define S4WD_CONFIGURATION_FILE               "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Config\\s4wd.ini"
        #define S4WD_DEBUG_FILE                       "C:\\Users\\Vaughn\\Workspace S4wd\\lcdmon\\Data\\Debug\\%1s4wd.dbg"

    #else
        #define CHARGE_PROFILE_LOG_DIR                "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Charge\\"
        #define DISCHARGE_PROFILE_LOG_DIR             "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Discharge\\"
        #define CHARGE_PROFILE_LOG_FILE               "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Charge\\%1\\dat.log"
        #define DISCHARGE_PROFILE_LOG_FILE            "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Discharge\\%1\\dat.log"
        #define S4WD_CONFIGURATION_FILE               "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Config\\s4wd.ini"
        #define S4WD_DEBUG_FILE                       "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Debug\\%1s4wd.dbg"
    #endif

#else
#define S4WD_CONFIGURATION_FILE               "/home/s4wd/config/s4wd.ini"
#define S4WD_CONFIGURATION_BAK_FILE           "/home/s4wd/config/s4wdbak.ini"
#define S4WD_DEBUG_FILE                       "/home/s4wd/config/%1s4wd.dbg"
#define CHARGE_PROFILE_LOG_DIR                "/home/s4wd/logging/Charge/"
#define DISCHARGE_PROFILE_LOG_DIR             "/home/s4wd/logging/Discharge/"
#define CHARGE_PROFILE_LOG_FILE               "/home/s4wd/logging/Charge/%1/dat.log"
#define DISCHARGE_PROFILE_LOG_FILE            "/home/s4wd/logging/Discharge/%1/dat.log"
#endif

class LoggerAgent : public QObject
{
    Q_OBJECT

public:
    LoggerAgent(QObject *parent = 0);
    ~LoggerAgent();

public slots:
    void SlotNewRecord(QStringList sample);
    void SlotStartNewLogSession(bool charge);

private:
    QString session;
    bool charging;

    void ProfileLog(QStringList record);

};

#endif // LOGGER_H
