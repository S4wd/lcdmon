#ifndef BUSAGENT
#define BUSAGENT

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QTimer>

#include "datastructs.h"





class BusAgent : public QObject
{
    Q_OBJECT

public:
    BusAgent( QObject *parent = 0 );
    ~BusAgent();
    void        BusInitialise(QString Address, QStringList SensorList, int thrno);

signals:
    void        SignalCallerBusNewTemps(QStringList Newtemps);
    void        SignalCallerBusInitialised(int thrno);

public slots:
    void        SlotCallerBusAccess( bool connect );


private slots:
    void        TempSampleTimerTimeout();

protected:
    int         ThreadNo;
    qint16      BusPortNumber;
    QString     BusAddress;
    QStringList Sensors;
    int         SensorCount;
    QTimer  *   TempSampleTimer;

    void        ReadTemperatures();
};

#endif // BUSAGENT

