#ifndef POWERAGENT_H
#define POWERAGENT_H


#include <QObject>
#include <QTime>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QSettings>



#ifdef Q_OS_WIN
#define POWER_LOG_FILE      "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Power\\%1power"
#define POWER_META_FILE      "C:\\Users\\vxc\\Workspace S4wd\\lcdmon\\Data\\Power\\meta"
#else
#define POWER_LOG_FILE      "/home/s4wd/logging/power/%1power"
#define POWER_META_FILE      "/home/s4wd/logging/power/meta"
#endif

struct DataRecord {
        QString datetime;
        float current;
        float voltage;
        float velocity;
        float altitude;
};




class PowerAgent : public QObject
{
    Q_OBJECT

public:
    PowerAgent(QObject *parent = 0);
    ~PowerAgent();

public slots:
    void SlotConfigure();

private slots:
    void SlotADSFinished(int exitstatus);
    void ADSTimerTimeout();

signals:

    void SignalNewReading( QStringList newreadings );

private:

    DataRecord newdata;

    QProcess * ADSProcess;
    QTimer * ADSTimer;
    DataRecord myData;

    void ADSRead();
};

#endif // POWERAGENT_H

