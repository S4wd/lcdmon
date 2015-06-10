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





#define POWER_LOG_FILE      "/home/s4wd/logging/power/%1power"
#define POWER_META_FILE      "/home/s4wd/logging/power/meta"

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
    void LogTimerTimeout();

signals:

    void SignalNewReading( QStringList newreadings );

private:
    bool loggingToFile;
    DataRecord newdata;
    qint16 samplesCaptured;
    qint16 filePrefix;
    QProcess * ADSProcess;
    QTimer * ADSTimer;
    QTimer * LogTimer;
    QVector<DataRecord> myData;

    void ADSRead();
    void getMeta();
    void setMeta();
    void LogToFile();
    void ProcessNewData();
};

#endif // POWERAGENT_H

