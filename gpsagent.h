#ifndef GPSAGENT_H
#define GPSAGENT_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QThread>

class GpsAgent : public QObject
{
    Q_OBJECT
public:
    explicit GpsAgent(QObject *parent = 0);
    ~GpsAgent();

signals:
    void SignalNewData(QStringList newReadings);

public slots:
    void SlotInitialise();

private slots:
    void ReadGpsTimerTimeout();
    void SlotProcessFinished(int, QProcess::ExitStatus);

private:
    QTimer * ReadGpsTimer;
    QProcess devTTyS;
};

#endif // GPSAGENT_H
