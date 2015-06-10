#ifndef IOAGENT_H
#define IOAGENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QTimer>

#define SIREN_TYPE_A1_PULSE         1
#define SIREN_TYPE_A2_CONSTANT      2

#define PCA9536_PATH            "/usr/src/qt/s4wd_test/PCA9536"

enum TSirenType {stA1Pulse, stA2Constant};
enum QPCACommand {pcaNone, pcaConfigure, pcaReadIgnition, pcaReadBatState, pcaA1On, pcaA1Off, pcaA2On, pcaA2Off};

class IOAgent : public QObject
{
    Q_OBJECT
public:
    explicit IOAgent(QObject *parent = 0);
    ~IOAgent();



signals:
    void SignalBat12VState(bool good);

private slots:
    void SlotPCAProcessFinished(int exitstatus);
    void SlotBatStateTimerTimeout();


public slots:
    void SlotConfigure();
    void SlotSirenOn(int sirenType);
    void SlotSirenOff(int sirenType);

private:
    bool Bat12VState;
    bool A1On;
    bool A2On;
    QPCACommand runningCmd;
    QList<QPCACommand> pcaCmd;

    QProcess * PCAProcess;
    QTimer * BatStateTimer;

    void RunQueudTask();

    void ReadBat12VState();
    void ConfigureIO();
    void AlarmA1On();
    void AlarmA1Off();
    void AlarmA2On();
    void AlarmA2Off();
};

#endif // IOAGENT_H
