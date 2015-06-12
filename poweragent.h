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

#define ADS1115_PATH            "/usr/src/s4wd/ADS1115"


struct DataRecord {
        float current;
        float voltage;
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

