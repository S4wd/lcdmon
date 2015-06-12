#include "ioagent.h"

IOAgent::IOAgent(QObject *parent) :
    QObject(parent)
{
    parent = 0;
    pcaCmd.clear();
    runningCmd = pcaNone;

    A1On = false;
    A2On = false;
    Bat12VState = true;
}

IOAgent::~IOAgent()
{
    delete PCAProcess;
}

void IOAgent::SlotPCAProcessFinished(int exitstatus)
{
    Q_UNUSED(exitstatus);

    if (runningCmd == pcaReadBatState)
    {

        if (PCAProcess->exitCode() == 1)
        {
            qDebug() << "Io Process: 12V good.";
            emit SignalBat12VState(true);
        }

        else if (PCAProcess->exitCode() == 0)
        {
            qDebug() << "Io Process: 12V bad.";
            emit SignalBat12VState(false);
        }
    }


    runningCmd = pcaNone;
    PCAProcess->close();


    if ( pcaCmd.isEmpty())
        return;

    RunQueudTask();


}

void IOAgent::SlotBatStateTimerTimeout()
{
    ReadBat12VState();
}

void IOAgent::SlotConfigure()
{
    qDebug() << "IO Thread ID:  " << QThread::currentThreadId();

    PCAProcess = new QProcess(this);
    QObject::connect(PCAProcess, SIGNAL(finished(int)), this, SLOT(SlotPCAProcessFinished(int)));

    BatStateTimer = new QTimer;
    QObject::connect(BatStateTimer, SIGNAL(timeout()), this, SLOT(SlotBatStateTimerTimeout()));
    BatStateTimer->start(300000);

    ConfigureIO();
    ReadBat12VState();
}



void IOAgent::RunQueudTask()
{
    QPCACommand newTask = pcaCmd.takeFirst();

    switch(newTask)
    {
        case pcaNone:
            break;

        case pcaConfigure:
            ConfigureIO();
            break;

        case pcaReadBatState:
            ReadBat12VState();
            break;

        case pcaA1On:
            AlarmA1On();
            break;

        case pcaA1Off:
            AlarmA1Off();
            break;

        case pcaA2On:
            AlarmA2On();
            break;

        case pcaA2Off:
            AlarmA1Off();
            break;

        default:
            break;
    }
}

void IOAgent::SlotSirenOn(int sirenType)
{
    if ( sirenType == SIREN_TYPE_A1_PULSE )
    {
        if (A1On)
            return;

        A1On = true;
        AlarmA1On();
    }
    else if ( sirenType == SIREN_TYPE_A2_CONSTANT )
    {
        if (A2On)
            return;

        A2On = true;
        AlarmA2On();
    }
}

void IOAgent::SlotSirenOff(int sirenType)
{
    if ( sirenType == SIREN_TYPE_A1_PULSE )
    {
        if (!A1On)
            return;

        A1On = false;
        AlarmA1Off();
    }
    else if ( sirenType == SIREN_TYPE_A2_CONSTANT )
    {
        if (!A2On)
            return;

        A2On = false;
        AlarmA2Off();
    }
}



void IOAgent::ConfigureIO()
{
#ifdef Q_OS_WIN
    qDebug() << "Configure IO.";

#else

    runningCmd = pcaConfigure;
    QString pcaapp = QString(PCA9536_PATH);
    QStringList argslist = QStringList() << "-c" << "2";
    qDebug() << "Configure IO.";
    PCAProcess->start(pcaapp,argslist);
#endif
}

void IOAgent::AlarmA1On()
{
#ifdef Q_OS_WIN
    qDebug() << "A1 on.";

#else
    if (PCAProcess->state()  != QProcess::NotRunning)
    {
        pcaCmd.append(pcaA1On);
        return;
    }

    runningCmd = pcaA1On;
    QString pcaapp = PCA9536_PATH;
    QStringList argslist = QStringList() << "-1" << "2";
    qDebug() << "A1 on.";
    PCAProcess->start(pcaapp,argslist);
#endif
}

void IOAgent::AlarmA1Off()
{
#ifdef Q_OS_WIN
    qDebug() << "A1 off.";

#else

    if (PCAProcess->state() != QProcess::NotRunning)
    {
        pcaCmd.append(pcaA1Off);
        return;
    }

    runningCmd = pcaA1Off;
    QString pcaapp = PCA9536_PATH;
    QStringList argslist = QStringList() << "-0" << "2";
    qDebug() << "A1 off.";
    PCAProcess->start(pcaapp,argslist);
#endif
}

void IOAgent::AlarmA2On()
{

#ifdef Q_OS_WIN
    qDebug() << "A2 on.";

#else
    if (PCAProcess->state()  != QProcess::NotRunning)
    {
        pcaCmd.append(pcaA2On);
        return;
    }

    runningCmd = pcaA2On;
    QString pcaapp = PCA9536_PATH;
    QStringList argslist = QStringList() << "-2" << "2";
    qDebug() << "A2 on.";
    PCAProcess->start(pcaapp,argslist);
#endif
}

void IOAgent::AlarmA2Off()
{
#ifdef Q_OS_WIN
    qDebug() << "A2 off.";

#else

    if (PCAProcess->state()  != QProcess::NotRunning)
    {
        pcaCmd.append(pcaA2Off);
        return;
    }

    runningCmd = pcaA2Off;
    QString pcaapp = PCA9536_PATH;
    QStringList argslist = QStringList() << "-0" << "2";
    qDebug() << "A2 off.";
    PCAProcess->start(pcaapp,argslist);
#endif
}

void IOAgent::ReadBat12VState()
{
#ifdef Q_OS_WIN
    qDebug() << "12V good.";
    emit SignalBat12VState(true);
#else

    if (PCAProcess->state()  != QProcess::NotRunning)
    {
        pcaCmd.append(pcaReadBatState);
        qDebug() << "Io process running. Cmd 12VbatRead queued.";
        return;
    }

    runningCmd = pcaReadBatState;
    QString pcaapp = PCA9536_PATH;
    QStringList argslist = QStringList() << "-i" << "2";

    // QProcess process;
    PCAProcess->start(pcaapp,argslist);
#endif
}
