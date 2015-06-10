#include "widget.h"
#include "ui_widget.h"

#include <QTime>



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    scene = new CustomScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->rotate(180);
    scene->setSceneRect(0,0,800,480);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "Main Thread ID:  " << QThread::currentThreadId();

    GetConfiguration();


    QObject::connect(&FlashTimer, SIGNAL(timeout()), this, SLOT(SlotFlashTimerTimeout()));

    TempLoggerSetup();
    IOAgentSetup();
    PowerAgentSetup();
    TemperatureSetup();

    scene->Initialise(tempDevice, &ioDevice, &powerDevice);


    ui->graphicsView->show();
}

Widget::~Widget()
{
    qDebug() << "Destroying widget";

    // terminate any running threads

    for (int i=0; i<4; i++)
    {
        if ( BusThread[i].isRunning() )
             BusThread[i].terminate();
    }

    if ( IOThread.isRunning() )
         IOThread.terminate();

    if ( LoggerThread.isRunning() )
        LoggerThread.terminate();

    if ( PowerThread.isRunning() )
        PowerThread.terminate();

    // delete
    delete scene;
    delete ui;
}


/* CONFIGURATION */
void Widget::GetConfiguration()
{
    QSettings settings(QString(S4WD_CONFIGURATION_FILE), QSettings::IniFormat);

    for (int i=0; i<=tdFrontAmbient; i++)
    {
        settings.beginGroup(QString("device%1").arg(i+1));
        tempDevice[i].Alias = settings.value(QString("Alias")).toString();
        tempDevice[i].SensorP.Serial = settings.value(QString("SerialP")).toString();
        tempDevice[i].SensorN.Serial = settings.value(QString("SerialN")).toString();
        tempDevice[i].SensorP.Alias = settings.value(QString("AliasP")).toString();
        tempDevice[i].SensorN.Alias = settings.value(QString("AliasN")).toString();
        tempDevice[i].Alarm1 = settings.value(QString("A1")).toFloat();
        tempDevice[i].Alarm2 = settings.value(QString("A2")).toFloat();
        qDebug() << tempDevice[i].Alias;
        qDebug() << QString("Alarm1: %1").arg(tempDevice[i].Alarm1);
        qDebug() << QString("Alarm2: %1").arg(tempDevice[i].Alarm2);
        settings.endGroup();
    }

    // bank volts
    settings.beginGroup(QString("bankvoltage"));
    powerDevice.Voltage.alarm1 = settings.value(QString("A1")).toFloat();
    powerDevice.Voltage.alarm2 = settings.value(QString("A2")).toFloat();
    qDebug() << QString("Alarm1: %1").arg(powerDevice.Voltage.alarm1);
    qDebug() << QString("Alarm2: %1").arg(powerDevice.Voltage.alarm2);
    settings.endGroup();

    // bank current
    settings.beginGroup(QString("bankcurrent"));
    powerDevice.Current.alarm1 = settings.value(QString("A1")).toFloat();
    powerDevice.Current.alarm2 = settings.value(QString("A2")).toFloat();
    qDebug() << QString("Alarm1: %1").arg(powerDevice.Current.alarm1);
    qDebug() << QString("Alarm2: %1").arg(powerDevice.Current.alarm2);
    settings.endGroup();
}












/* NEW TEMPERATURE READINGS FROM BUS THREAD */
void Widget::SlotBus1NewTemps(QStringList newtemps)
{
    // Order: (1) controller case (2) BF1+ (3) BF2+ (4) BF3+ (5) BF4+ (6) Motor case (7) Ambient RHS
    tempDevice[tdController].SensorN.T = newtemps.at(0).toFloat();
    HIGHER_TEMP(tempDevice[tdController].HighTemp, tempDevice[tdController].SensorP.T, tempDevice[tdController].SensorN.T);

    tempDevice[tdBatA].SensorP.T = newtemps.at(1).toFloat();
    HIGHER_TEMP(tempDevice[tdBatA].HighTemp, tempDevice[tdBatA].SensorP.T, tempDevice[tdBatA].SensorN.T);

    tempDevice[tdBatB].SensorP.T = newtemps.at(2).toFloat();
    HIGHER_TEMP(tempDevice[tdBatB].HighTemp, tempDevice[tdBatB].SensorP.T, tempDevice[tdBatB].SensorN.T);

    tempDevice[tdBatC].SensorP.T = newtemps.at(3).toFloat();
    HIGHER_TEMP(tempDevice[tdBatC].HighTemp, tempDevice[tdBatC].SensorP.T, tempDevice[tdBatC].SensorN.T);

    tempDevice[tdBatD].SensorP.T = newtemps.at(4).toFloat();
    HIGHER_TEMP(tempDevice[tdBatD].HighTemp, tempDevice[tdBatD].SensorP.T, tempDevice[tdBatD].SensorN.T);

    tempDevice[tdMotor].SensorN.T = newtemps.at(5).toFloat();
    HIGHER_TEMP(tempDevice[tdMotor].HighTemp, tempDevice[tdMotor].SensorP.T, tempDevice[tdMotor].SensorN.T);

    tempDevice[tdFrontAmbient].SensorP.T = newtemps.at(6).toFloat();
    HIGHER_TEMP(tempDevice[tdFrontAmbient].HighTemp, tempDevice[tdFrontAmbient].SensorP.T, tempDevice[tdFrontAmbient].SensorN.T);

    // update alarm status
    DevicesUpdateAlarmStatus();
}

void Widget::SlotBus2NewTemps(QStringList newtemps)
{
    // Order: (1) Front ambient LHS (2) Controller (3) BF2- (4) BF1- (5) BF3- (6) BF4- (7) Motor
    tempDevice[tdFrontAmbient].SensorN.T = newtemps.at(0).toFloat();
    HIGHER_TEMP(tempDevice[tdFrontAmbient].HighTemp, tempDevice[tdFrontAmbient].SensorP.T, tempDevice[tdFrontAmbient].SensorN.T);

    tempDevice[tdController].SensorP.T = newtemps.at(1).toFloat();
    HIGHER_TEMP(tempDevice[tdController].HighTemp, tempDevice[tdController].SensorP.T, tempDevice[tdController].SensorN.T);

    tempDevice[tdBatB].SensorN.T = newtemps.at(2).toFloat();
    HIGHER_TEMP(tempDevice[tdBatB].HighTemp, tempDevice[tdBatB].SensorP.T, tempDevice[tdBatB].SensorN.T);

    tempDevice[tdBatA].SensorN.T = newtemps.at(3).toFloat();
    HIGHER_TEMP(tempDevice[tdBatA].HighTemp, tempDevice[tdBatA].SensorP.T, tempDevice[tdBatA].SensorN.T);

    tempDevice[tdBatC].SensorN.T = newtemps.at(4).toFloat();
    HIGHER_TEMP(tempDevice[tdBatC].HighTemp, tempDevice[tdBatC].SensorP.T, tempDevice[tdBatC].SensorN.T);

    tempDevice[tdBatD].SensorN.T = newtemps.at(5).toFloat();
    HIGHER_TEMP(tempDevice[tdBatD].HighTemp, tempDevice[tdBatD].SensorP.T, tempDevice[tdBatD].SensorN.T);

    tempDevice[tdMotor].SensorP.T = newtemps.at(6).toFloat();
    HIGHER_TEMP(tempDevice[tdMotor].HighTemp, tempDevice[tdMotor].SensorP.T, tempDevice[tdMotor].SensorN.T);

    // update alarm status
    DevicesUpdateAlarmStatus();
}

void Widget::SlotBus3NewTemps(QStringList newtemps)
{
    // Order: (1) RA1+ (2) RA2+ (3) RA3+ (4) RA4+ (5) RB4+ (6) RB3+ (7) RB2+ (8) RB1+ (9) Back Ambient 1
    tempDevice[tdBatE].SensorP.T = newtemps.at(0).toFloat();
    HIGHER_TEMP(tempDevice[tdBatE].HighTemp, tempDevice[tdBatE].SensorP.T, tempDevice[tdBatE].SensorN.T);

    tempDevice[tdBatF].SensorP.T = newtemps.at(1).toFloat();
    HIGHER_TEMP(tempDevice[tdBatF].HighTemp, tempDevice[tdBatF].SensorP.T, tempDevice[tdBatF].SensorN.T);

    tempDevice[tdBatG].SensorP.T = newtemps.at(2).toFloat();
    HIGHER_TEMP(tempDevice[tdBatG].HighTemp, tempDevice[tdBatG].SensorP.T, tempDevice[tdBatG].SensorN.T);

    tempDevice[tdBatH].SensorP.T = newtemps.at(3).toFloat();
    HIGHER_TEMP(tempDevice[tdBatH].HighTemp, tempDevice[tdBatH].SensorP.T, tempDevice[tdBatH].SensorN.T);

    tempDevice[tdBatL].SensorP.T = newtemps.at(4).toFloat();
    HIGHER_TEMP(tempDevice[tdBatL].HighTemp, tempDevice[tdBatL].SensorP.T, tempDevice[tdBatL].SensorN.T);

    tempDevice[tdBatK].SensorP.T = newtemps.at(5).toFloat();
    HIGHER_TEMP(tempDevice[tdBatK].HighTemp, tempDevice[tdBatK].SensorP.T, tempDevice[tdBatK].SensorN.T);

    tempDevice[tdBatJ].SensorP.T = newtemps.at(6).toFloat();
    HIGHER_TEMP(tempDevice[tdBatJ].HighTemp, tempDevice[tdBatJ].SensorP.T, tempDevice[tdBatJ].SensorN.T);

    tempDevice[tdBatI].SensorP.T = newtemps.at(7).toFloat();
    HIGHER_TEMP(tempDevice[tdBatI].HighTemp, tempDevice[tdBatI].SensorP.T, tempDevice[tdBatI].SensorN.T);

    tempDevice[tdBackAmbient].SensorP.T = newtemps.at(8).toFloat();
    HIGHER_TEMP(tempDevice[tdBackAmbient].HighTemp, tempDevice[tdBackAmbient].SensorP.T, tempDevice[tdBackAmbient].SensorN.T);

    // update alarm status
    DevicesUpdateAlarmStatus();
}

void Widget::SlotBus4NewTemps(QStringList newtemps)
{
    // Order: (1) RA1- (2) RA2- (3) RA3- (4) RA4- (5) RB4- (6) RB3- (7) RB2- (8) RB1- (9) Back Ambient 2
    tempDevice[tdBatE].SensorN.T = newtemps.at(0).toFloat();
    HIGHER_TEMP(tempDevice[tdBatE].HighTemp, tempDevice[tdBatE].SensorP.T, tempDevice[tdBatE].SensorN.T);

    tempDevice[tdBatF].SensorN.T = newtemps.at(1).toFloat();
    HIGHER_TEMP(tempDevice[tdBatF].HighTemp, tempDevice[tdBatF].SensorP.T, tempDevice[tdBatF].SensorN.T);

    tempDevice[tdBatG].SensorN.T = newtemps.at(2).toFloat();
    HIGHER_TEMP(tempDevice[tdBatG].HighTemp, tempDevice[tdBatG].SensorP.T, tempDevice[tdBatG].SensorN.T);

    tempDevice[tdBatH].SensorN.T = newtemps.at(3).toFloat();
    HIGHER_TEMP(tempDevice[tdBatH].HighTemp, tempDevice[tdBatH].SensorP.T, tempDevice[tdBatH].SensorN.T);

    tempDevice[tdBatL].SensorN.T = newtemps.at(4).toFloat();
    HIGHER_TEMP(tempDevice[tdBatL].HighTemp, tempDevice[tdBatL].SensorP.T, tempDevice[tdBatL].SensorN.T);

    tempDevice[tdBatK].SensorN.T = newtemps.at(5).toFloat();
    HIGHER_TEMP(tempDevice[tdBatK].HighTemp, tempDevice[tdBatK].SensorP.T, tempDevice[tdBatK].SensorN.T);

    tempDevice[tdBatJ].SensorN.T = newtemps.at(6).toFloat();
    HIGHER_TEMP(tempDevice[tdBatJ].HighTemp, tempDevice[tdBatJ].SensorP.T, tempDevice[tdBatJ].SensorN.T);

    tempDevice[tdBatI].SensorN.T = newtemps.at(7).toFloat();
    HIGHER_TEMP(tempDevice[tdBatI].HighTemp, tempDevice[tdBatI].SensorP.T, tempDevice[tdBatI].SensorN.T);

    tempDevice[tdBackAmbient].SensorN.T = newtemps.at(8).toFloat();
    HIGHER_TEMP(tempDevice[tdBackAmbient].HighTemp, tempDevice[tdBackAmbient].SensorP.T, tempDevice[tdBackAmbient].SensorN.T);

    // update alarm status
    DevicesUpdateAlarmStatus();
}

void Widget::SlotPowerNewReadings(QStringList readings)
{
    if (readings.count() != 2)
        return;

    float current = readings[0].toFloat();
    if (current >= 0 && current < 401)
        powerDevice.Current.reading = current;


    float voltage = readings[1].toFloat();
    if (voltage >= 40 && current < 52.1)
        powerDevice.Voltage.reading = voltage;

    DevicesUpdateAlarmStatus();
}

void Widget::SlotIOIgnitionState(bool state)
{
    ioDevice.ignition = state;
    TDisplayView currentView = scene->GetView();
    if (currentView == dvDriver && !ioDevice.ignition)
        scene->SetView(dvBatteryBank);
    else if (currentView == dvBatteryBank && ioDevice.ignition)
        scene->SetView(dvDriver);
}

void Widget::SlotIOBat12VState(bool state)
{
    ioDevice.Bat12VGood = state;
    scene->SetBat12VState(state);
}



/* TIMERS */

void Widget::SlotFlashTimerTimeout()
{
    bool MuteStatus = scene->MuteIsActive();

    if (MuteStatus)
    {
        SignalIOSirenOff(SIREN_TYPE_A1_PULSE);
        SignalIOSirenOff(SIREN_TYPE_A2_CONSTANT);
    }


    for (int i=tdBatA; i<=tdFrontAmbient; i++)
    {
        if ( tempDevice[i].AlarmStatus != asNone )
            scene->DisplayFlashNow(i);
    }

    if ( powerDevice.Voltage.status != asNone )
        scene->DisplayFlashNow(tdBankVoltage);

    if ( powerDevice.Current.status != asNone )
        scene->DisplayFlashNow(tdBankCurrent);
}

void Widget::SlotIOBat12VTimerTimeout()
{

}



void Widget::SlotLoggingTimerTimeout()
{
    QString s;
    QStringList Record;

    // temperatures
    for (int i=tdBatA; i<=tdFrontAmbient; i++)
        Record << QString("%1").arg( s.number( tempDevice[i].HighTemp, 'f', 1) );

    emit SignalLoggingNewRecord(Record);
}







void Widget::DevicesUpdateAlarmStatus()
{
    bool A1Visual = false;
    bool A2Visual = false;
    bool A1Audio = false;
    bool A2Audio = false;

    //qDebug() << "Update alarm status";
    for (int i=tdBatA; i<=tdFrontAmbient; i++)
    {
        scene->DisplaySetValue(i);

        // alarm status
        if ( tempDevice[i].HighTemp >= tempDevice[i].Alarm2 )
        {
            tempDevice[i].AlarmStatus = asAlarm2;
            scene->DisplayStartFlashing(asAlarm2, i);
            A2Visual = true;


            if (!tempDevice[i].a2muted)
                A2Audio = true;
        }

        else if ( tempDevice[i].HighTemp >= tempDevice[i].Alarm1 )
        {
            tempDevice[i].AlarmStatus = asAlarm1;
            scene->DisplayStartFlashing(asAlarm1, i);
            A1Visual = true;

            if (!tempDevice[i].a1muted && !tempDevice[i].a2muted)
                A1Audio = true;
        }

        else
        {
            scene->DisplayStopFlashing(i);
            tempDevice[i].AlarmStatus = asNone;
        }
    }

    // bank voltage
    scene->DisplaySetValue(tdBankVoltage);

    if ( powerDevice.Voltage.reading <=  powerDevice.Voltage.alarm2 )
    {
        powerDevice.Voltage.status = asAlarm2;
        scene->DisplayStartFlashing(asAlarm2, tdBankVoltage);

        A2Visual = true;

        if (!powerDevice.Voltage.a2muted)
            A2Audio = true;
    }

    else if ( powerDevice.Voltage.reading <= powerDevice.Voltage.alarm1 )
    {
        powerDevice.Voltage.status = asAlarm1;
        scene->DisplayStartFlashing(asAlarm1, tdBankVoltage);
        A1Visual = true;

        if (!powerDevice.Voltage.a1muted && !powerDevice.Voltage.a2muted)
            A1Audio = true;
    }

    else
    {
        scene->DisplayStopFlashing(tdBankVoltage);
        powerDevice.Voltage.status = asNone;
    }

    // bank current
    scene->DisplaySetValue(tdBankCurrent);
    if ( powerDevice.Current.reading >=  powerDevice.Current.alarm2 )
    {
        powerDevice.Current.status = asAlarm2;
        scene->DisplayStartFlashing(asAlarm2, tdBankCurrent);
        A2Visual = true;

        if (!powerDevice.Current.a2muted)
            A2Audio = true;
    }

    else if ( powerDevice.Current.reading >= powerDevice.Current.alarm1 )
    {
        powerDevice.Current.status = asAlarm1;
        scene->DisplayStartFlashing(asAlarm1, tdBankCurrent);
        A1Visual = true;
        if (!powerDevice.Current.a1muted && !powerDevice.Current.a2muted)
            A2Audio = true;
    }

    else
    {
        scene->DisplayStopFlashing(tdBankCurrent);
        powerDevice.Current.status = asNone;
    }


    //MuteStatus = scene->MuteIsActive();
    // alert

    if (A2Visual || A1Visual)
    {
        if ( !FlashTimer.isActive() )
            FlashTimer.start(1000);
    }
    else if ( FlashTimer.isActive() )
        FlashTimer.stop();

    if (A2Audio)
        SignalIOSirenOn(SIREN_TYPE_A2_CONSTANT);
    else if (A1Audio)
        SignalIOSirenOn(SIREN_TYPE_A1_PULSE);
    else
    {
        SignalIOSirenOff(SIREN_TYPE_A1_PULSE);
        SignalIOSirenOff(SIREN_TYPE_A2_CONSTANT);
    }

}


void Widget::TempLoggerSetup()
{
    QObject::connect(&LoggingTimer, SIGNAL(timeout()), this, SLOT(SlotLoggingTimerTimeout()));

    TempLogger.moveToThread(&LoggerThread);
    QObject::connect(this, SIGNAL(SignalLoggingNewRecord(QStringList)), &TempLogger, SLOT(SlotNewRecord(QStringList)));
    LoggerThread.start();

    LoggingTimer.start(60000);
}

void Widget::PowerAgentSetup()
{
    powerDevice.Current.reading = 0.0;
    powerDevice.Current.status = asNone;
    powerDevice.Current.a1muted = false;
    powerDevice.Current.a2muted = false;

    powerDevice.Voltage.reading = 0.0;
    powerDevice.Voltage.status = asNone;
    powerDevice.Voltage.a1muted = false;
    powerDevice.Voltage.a2muted = false;

    Power.moveToThread(&PowerThread);
    QObject::connect(this, SIGNAL(SignalPowerConfigure()), &Power, SLOT(SlotConfigure()) );
    QObject::connect(&Power, SIGNAL(SignalNewReading(QStringList)), this, SLOT(SlotPowerNewReadings(QStringList)));

    PowerThread.start();
    emit SignalPowerConfigure();

}

void Widget::IOAgentSetup()
{
    ioDevice.Bat12VMuted = false;
    ioDevice.Bat12VGood = true;
    ioDevice.ignition = false;

    InputOutput.moveToThread(&IOThread);
    QObject::connect(this, SIGNAL(SignalIOConfigure()), &InputOutput, SLOT(SlotConfigure()));
    //QObject::connect(this, SIGNAL(SignalIOBat12VState()), &InputOutput, SLOT(SlotBat12VState()));
    //QObject::connect(this, SIGNAL(SignalIOIgnitionState()), &InputOutput, SLOT(SlotIgnitionState()));
    QObject::connect(this, SIGNAL(SignalIOSirenOn(int)), &InputOutput, SLOT(SlotSirenOn(int)));
    QObject::connect(this, SIGNAL(SignalIOSirenOff(int)), &InputOutput, SLOT(SlotSirenOff(int)));

    //QObject::connect(&InputOutput, SIGNAL(SignalIgnitionState(bool)), this, SLOT(SlotIOIgnitionState(bool)));
    QObject::connect(&InputOutput, SIGNAL(SignalBat12VState(bool)), this, SLOT(SlotIOBat12VState(bool)));



    IOThread.start();
    emit SignalIOConfigure();

}

void Widget::TemperatureSetup()
{

    int i;
    TempDevice device;
    QStringList sensorList;


    // setup temperature tempDevice
    device.SensorP.T = (float)0;
    device.SensorN.T = (float)0;
    device.SensorP.T = (float)0;
    device.SensorN.T  = (float)0;
    device.HighTemp = (float)0;
    device.AlarmStatus = asNone;
    device.a1muted = false;
    device.a2muted = false;


    // batteries
    for (i=tdBatA; i<=tdFrontAmbient; i++)
    {
        // copy alias, alarms first else A1/A2 will be 0 when tempDevice[i] = device is executed
        device.Alias = tempDevice[i].Alias;
        device.Alarm1 = tempDevice[i].Alarm1;
        device.Alarm2 = tempDevice[i].Alarm2;
        device.SensorP.Alias = tempDevice[i].SensorP.Alias;
        device.SensorN.Alias = tempDevice[i].SensorN.Alias;
        device.SensorP.Serial = tempDevice[i].SensorP.Serial;
        device.SensorN.Serial = tempDevice[i].SensorN.Serial;
        tempDevice[i] = device;
    }



    /* Bus 1 */
    // Order: (1) controller case (2) BF1+ (3) BF2+ (4) BF3+ (5) BF4+ (6) Motor case (7) Ambient RHS
    sensorList.clear();
    sensorList.append(tempDevice[tdController].SensorN.Serial);
    sensorList.append(tempDevice[tdBatA].SensorP.Serial);
    sensorList.append(tempDevice[tdBatB].SensorP.Serial);
    sensorList.append(tempDevice[tdBatC].SensorP.Serial);
    sensorList.append(tempDevice[tdBatD].SensorP.Serial);
    sensorList.append(tempDevice[tdMotor].SensorN.Serial);
    sensorList.append(tempDevice[tdFrontAmbient].SensorP.Serial);
    //qDebug() << sensorList[0];

    Buses[0].BusInitialise(busAddress[0], sensorList,1);
    // move agent to thread
    Buses[0].moveToThread(&BusThread[0]);

    // caller signals to agent slots
    QObject::connect(this, SIGNAL(SignalBus1Configure(bool)), &Buses[0], SLOT(SlotCallerBusAccess(bool)));

    // agent signals to caller slots
    QObject::connect(&Buses[0], SIGNAL(SignalCallerBusNewTemps(QStringList)), this, SLOT(SlotBus1NewTemps(QStringList)));
    BusThread[0].start();
    emit SignalBus1Configure(true);



    /* Bus 2 */
    sensorList.clear();
    // Order: (1) Front ambient LHS (2) Controller (3) BF2- (4) BF1- (5) BF3- (6) BF4- (7) Motor
    sensorList.append(tempDevice[tdFrontAmbient].SensorN.Serial);
    sensorList.append(tempDevice[tdController].SensorP.Serial);
    sensorList.append(tempDevice[tdBatB].SensorN.Serial);
    sensorList.append(tempDevice[tdBatA].SensorN.Serial);
    sensorList.append(tempDevice[tdBatC].SensorN.Serial);
    sensorList.append(tempDevice[tdBatD].SensorN.Serial);
    sensorList.append(tempDevice[tdMotor].SensorP.Serial);

    Buses[1].BusInitialise(busAddress[1], sensorList,2);
    // move agent to thread
    Buses[1].moveToThread(&BusThread[1]);

    // widget signals to bus object slots
    QObject::connect(this, SIGNAL(SignalBus2Configure(bool)), &Buses[1], SLOT(SlotCallerBusAccess(bool)));


    // agent signals to caller slots
    QObject::connect(&Buses[1], SIGNAL(SignalCallerBusNewTemps(QStringList)), this, SLOT(SlotBus2NewTemps(QStringList)));
    BusThread[1].start();
    emit SignalBus2Configure(true);



    /* Bus 3 */
    sensorList.clear();
    // Order: (1) RA1+ (2) RA2+ (3) RA3+ (4) RA4+ (5) RB4+ (6) RB3+ (7) RB2+ (8) RB1+ (9) Back Ambient 1
    sensorList.append(tempDevice[tdBatE].SensorP.Serial);
    sensorList.append(tempDevice[tdBatF].SensorP.Serial);
    sensorList.append(tempDevice[tdBatG].SensorP.Serial);
    sensorList.append(tempDevice[tdBatH].SensorP.Serial);
    sensorList.append(tempDevice[tdBatL].SensorP.Serial);
    sensorList.append(tempDevice[tdBatK].SensorP.Serial);
    sensorList.append(tempDevice[tdBatJ].SensorP.Serial);
    sensorList.append(tempDevice[tdBatI].SensorP.Serial);
    sensorList.append(tempDevice[tdBackAmbient].SensorP.Serial);
    Buses[2].BusInitialise(busAddress[2], sensorList,3);
    // move agent to thread
    Buses[2].moveToThread(&BusThread[2]);

    // widget signals to bus object slots
    QObject::connect(this, SIGNAL(SignalBus3Configure(bool)), &Buses[2], SLOT(SlotCallerBusAccess(bool)));


    // agent signals to caller slots
    QObject::connect(&Buses[2], SIGNAL(SignalCallerBusNewTemps(QStringList)), this, SLOT(SlotBus3NewTemps(QStringList)));
    BusThread[2].start();
    emit SignalBus3Configure(true);


    /* Bus 4 */
    sensorList.clear();
    // Order: (1) RA1- (2) RA2- (3) RA3- (4) RA4- (5) RB4- (6) RB3- (7) RB2- (8) RB1- (9) Back Ambient 2
    sensorList.append(tempDevice[tdBatE].SensorN.Serial);
    sensorList.append(tempDevice[tdBatF].SensorN.Serial);
    sensorList.append(tempDevice[tdBatG].SensorN.Serial);
    sensorList.append(tempDevice[tdBatH].SensorN.Serial);
    sensorList.append(tempDevice[tdBatL].SensorN.Serial);
    sensorList.append(tempDevice[tdBatK].SensorN.Serial);
    sensorList.append(tempDevice[tdBatJ].SensorN.Serial);
    sensorList.append(tempDevice[tdBatI].SensorN.Serial);
    sensorList.append(tempDevice[tdBackAmbient].SensorN.Serial);
    Buses[3].BusInitialise(busAddress[3], sensorList,4);
    // move agent to thread
    Buses[3].moveToThread(&BusThread[3]);

    // caller signals to agent slots
    QObject::connect(this, SIGNAL(SignalBus4Configure(bool)), &Buses[3], SLOT(SlotCallerBusAccess(bool)));


    // widget signals to bus object slots
    QObject::connect(&Buses[3], SIGNAL(SignalCallerBusNewTemps(QStringList)), this, SLOT(SlotBus4NewTemps(QStringList)));
    BusThread[3].start();
    emit SignalBus4Configure(true);


}







