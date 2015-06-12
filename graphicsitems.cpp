#include "graphicsitems.h"


























AlarmSpinnerItem::AlarmSpinnerItem()
{
    Value = 0;
    UpArrow = bsReleased;
    DownArrow = bsReleased;
    Resolution = 0.5;
    Unit = suTemperature;
    UpperBound = 125.0;
    LowerBound = -55.0;
}

AlarmSpinnerItem::~AlarmSpinnerItem()
{

}

QRectF AlarmSpinnerItem::boundingRect() const
{
    return QRectF(0,0,250,350);
}

void AlarmSpinnerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString s;

    // draw panel
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),10,10);

    QPen pen;
    pen.setColor(QColor(40,40,40));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QColor(60,60,60,100));
    painter->drawPath(frame);

    // up arrow
    QPainterPath upArrowFrame;
    QPixmap uparrow(PNG_IMAGE_UP);
    upArrowFrame.addRoundedRect(QRectF(5,5,240,100),5,5);
    if (UpArrow == bsPressed)
    {
        painter->setBrush(QColor(20,20,20,200));
        painter->drawPath(upArrowFrame);
        painter->drawPixmap(98,33,48,48,uparrow);
    }
    else if (UpArrow == bsReleased)
    {
        painter->setBrush(QColor(50,50,50,200));
        painter->drawPath(upArrowFrame);
        painter->drawPixmap(100,31,48,48,uparrow);
    }





    // down arrow
    QPainterPath downArrowFrame;
    QPixmap downarrow(PNG_IMAGE_DOWN);
    downArrowFrame.addRoundedRect(QRectF(5,245,240,100),5,5);
    if (DownArrow == bsPressed)
    {
        painter->setBrush(QColor(20,20,20,200));
        painter->drawPath(downArrowFrame);
        painter->drawPixmap(98,273,48,48,downarrow);
    }
    else if (DownArrow == bsReleased)
    {
        painter->setBrush(QColor(50,50,50,200));
        painter->drawPath(downArrowFrame);
        painter->drawPixmap(100,271,48,48,downarrow);
    }



    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(35);
    pen.setColor(QColor(Qt::white));
    painter->setFont(font);
    painter->setPen(pen);

    if (Unit == suTemperature)
        painter->drawText( QPointF(5,195), QString("%1%2C").arg(s.number( Value, 'f', 1)).arg(DEGREE));

    else if (Unit == suVoltage)
        painter->drawText( QPointF(5,195), QString("%1V").arg(s.number( Value, 'f', 1)));

    else if (Unit == suCurrent)
        painter->drawText( QPointF(5,195), QString("%1A").arg(s.number( Value, 'f', 0)));

}

float AlarmSpinnerItem::getValue()
{
    return Value;
}

void AlarmSpinnerItem::setValue(float value)
{
    Value = value;
}

void AlarmSpinnerItem::setParameters(TSpinnerUnit unit, float resolution, float low, float high, TSpinnerType spintype)
{
    Unit = unit;
    Resolution = resolution;
    LowerBound = low;
    UpperBound = high;
    SpinType = spintype;
}

TSpinnerAccess AlarmSpinnerItem::spinnerAccess(QPointF mousepoint)
{
    TSpinnerAccess spinner;
    QPointF ItemPos = mapFromScene(mousepoint);
    qreal ypos = ItemPos.y();

    if( ! contains(ItemPos) )
    {
        spinner = spNoArrow;
        return spinner;
    }


    if ( ypos <= 100 )
        spinner = spUpArrow;
    else if ( ypos > 244 && ypos <= 350 )
        spinner = spDownArrow;
    else
        spinner = spNoArrow;

    return spinner;
}


void AlarmSpinnerItem::changeValue(bool increment, bool fastscroll, float adjValue)
{

    if (!increment)
    {
        Value -= Resolution;

        if ( Value <= LowerBound )
            Value = LowerBound;

        else if ( SpinType == stA2 && Value <= adjValue && Unit != suVoltage)
            Value = adjValue;

        else if ( SpinType == stA1 && Value <= adjValue && Unit == suVoltage)
            Value = adjValue;

        if (!fastscroll)
            DownArrow = bsReleased;
    }

    // swiping down, so increase
    else
    {
        Value += Resolution;

        if ( Value >= UpperBound )
            Value = UpperBound;

        else if ( SpinType == stA1 && Value >= adjValue && Unit != suVoltage )
            Value = adjValue;

        else if ( SpinType == stA2 && Value >= adjValue && Unit == suVoltage)
            Value = adjValue;

        if (!fastscroll)
            UpArrow = bsReleased;
    }

    update();
}

void AlarmSpinnerItem::upArrowState(TButtonState bs)
{
    UpArrow = bs;
    update();
}

void AlarmSpinnerItem::downArrowState(TButtonState bs)
{
    DownArrow = bs;
    update();
}













CustomScene::CustomScene(QObject *parent)
{
    setParent(parent);


    GetConfiguration();


    QObject::connect(&FlashTimer, SIGNAL(timeout()), this, SLOT(SlotFlashTimerTimeout()));
    QObject::connect(&DataCaptureTimer, SIGNAL(timeout()), this, SLOT(SlotDataCaptureTimerTimeout()));


    LoggerSetup();
    GpsSetup();
    IOAgentSetup();
    PowerAgentSetup();
    TemperatureSetup();
    emit SignalBus1Configure(true);


    MuteActive = false;
    WifiActive = false;

    BvMuteButton = new ToggleButtonItem(QPixmap(PNG_IMAGE_MUTE_OFF),QPixmap(PNG_IMAGE_MUTE_ON));
    DvMuteButton = new ToggleButtonItem(QPixmap(PNG_IMAGE_MUTE_OFF),QPixmap(PNG_IMAGE_MUTE_ON));
    TickButton = new ButtonItem(QPixmap(PNG_IMAGE_TICK));
    BatBankViewButton = new ButtonItem(QPixmap(PNG_IMAGE_BATCHARGE));
    ConfigButton = new ButtonItem(QPixmap(PNG_IMAGE_CONFIG));
    ReturnButton = new ButtonItem(QPixmap(PNG_IMAGE_RETURN));
    DriveViewButton = new ButtonItem(QPixmap(PNG_IMAGE_GAUGE));
    ShutDownButton = new ButtonItem(QPixmap(PNG_IMAGE_SHUTDOWN));
    DVShutdownButton = new ButtonItem(QPixmap(PNG_IMAGE_SHUTDOWN));
    BVShutdownButton= new ButtonItem(QPixmap(PNG_IMAGE_SHUTDOWN));
    ShutdownReturnButton = new ButtonItem(QPixmap(PNG_IMAGE_RETURN));
    WifiApButton = new ToggleButtonItem(QPixmap(PNG_IMAGE_WIFI_OFF), QPixmap(PNG_IMAGE_WIFI_ON));

    QObject::connect(&SpinnerHoldDownTimer, SIGNAL(timeout()), this, SLOT(SlotSpinnerDownTimerTimeout()));
    QObject::connect(&SpinnerHoldDownActiveTimer, SIGNAL(timeout()), this, SLOT(SlotSpinnerDownActiveTimerTimeout()));


    LoadDriverDisplay();
    LoadBatteryBankDisplay();
    LoadTemperatureDisplay();
    LoadAlarmConfigDisplay();
    LoadShutDownView();

    // default display (dummy setting)
    CurrentView = &DrivingView;
    ChangeView(&DrivingView);


    ChargeProfileActive = true;
    moving = 0;
    DataCaptureTimer.start(CHARGE_PROFILE_DATA_CAPTURE_TIME);
}

CustomScene::~CustomScene()
{
    qDebug() << "Destroying scene";

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

    delete DvMuteButton;
    delete BvMuteButton;
    delete TickButton;
    delete BatBankViewButton;
    delete ConfigButton;
    delete ReturnButton;
    delete DriveViewButton;
    delete ShutDownButton;
    delete DVShutdownButton;
    delete BVShutdownButton;
    delete ShutdownReturnButton;
    delete WifiApButton;
}



void CustomScene::SetView(TDisplayView view)
{
    if (view == dvDriver)
        ChangeView(&DrivingView);
    else if (view == dvBatteryBank)
        ChangeView(&BatteryBankView);
}








TDisplayView CustomScene::GetView()
{
    if (CurrentView == &BatteryBankView)
        return dvBatteryBank;
    else if (CurrentView == &DrivingView)
        return dvDriver;
    else if (CurrentView == &AlarmConfigView)
        return dvAlarmConfig;
    else if (CurrentView == &HighDefTempView)
        return dvTemp;
    else
        return dvNotSetYet;
}

void CustomScene::DisplaySetValue(int display)
{
    if (display <= tdBatL)
        BatT[display].setValue(tempDevice[display].HighTemp);

    else if (display == tdMotor)
        Motor.setValue(tempDevice[tdMotor].HighTemp);

    else if (display == tdController)
        PwmC.setValue(tempDevice[tdController].HighTemp);

    else if (display == tdFrontAmbient)
        FAmb.setValue(tempDevice[tdFrontAmbient].HighTemp);

    else if (display == tdBackAmbient)
        BAmb.setValue(tempDevice[tdBackAmbient].HighTemp);



    else if (display == tdBankVoltage)
    {
        VoltGauge.setValue(powerDevice.Voltage.reading);
        vNeedle.setValue(powerDevice.Voltage.reading);
    }

    else if (display == tdBankCurrent)
    {
        AmpGauge.setValue(powerDevice.Current.reading);
        aNeedle.setValue(powerDevice.Current.reading);
    }

    if ( CurrentView == &HighDefTempView && display == DisplaySettingsDeviceIndex )
    {
        HighDefTempView.SetValues(tempDevice[display].SensorP.T,
                                  tempDevice[display].SensorN.T,
                                  tempDevice[display].Alarm1,
                                  tempDevice[display].Alarm2);
        HighDefTempView.UpdateView();
    }
}

void CustomScene::DisplayStartFlashing(TAlarmState aEvent, int display)
{       
    if (display <= tdBatL)
        BatT[display].flashStart(aEvent);

    else if (display == tdMotor)
        Motor.flashStart(aEvent);

    else if (display == tdController)
        PwmC.flashStart(aEvent);

    else if (display == tdFrontAmbient)
        FAmb.flashStart(aEvent);

    else if (display == tdBackAmbient)
        BAmb.flashStart(aEvent);

    else if (display == tdBankVoltage)
        VoltGauge.flashStart(aEvent);

    else if (display == tdBankCurrent)
        AmpGauge.flashStart(aEvent);
}

void CustomScene::DisplayStopFlashing(int display)
{
    if (display <= tdBatL)
        BatT[display].flashStop();

    else if (display == tdMotor)
        Motor.flashStop();

    else if (display == tdController)
        PwmC.flashStop();

    else if (display == tdFrontAmbient)
        FAmb.flashStop();

    else if (display == tdBackAmbient)
        BAmb.flashStop();

    else if (display == tdBankVoltage)
        VoltGauge.flashStop();

    else if (display == tdBankCurrent)
        AmpGauge.flashStop();
}

void CustomScene::DisplayFlashNow(int display)
{      
    if (display <= tdBatL)
        BatT[display].flashNow();

    else if (display == tdMotor)
        Motor.flashNow();

    else if (display == tdController)
        PwmC.flashNow();

    else if (display == tdFrontAmbient)
        FAmb.flashNow();

    else if (display == tdBackAmbient)
        BAmb.flashNow();

    else if (display == tdBankVoltage)
        VoltGauge.flashNow();

    else if (display == tdBankCurrent)
        AmpGauge.flashNow();
}

void CustomScene::LoadDriverDisplay()
{
    addItem(&DrivingView);
    DrivingView.setPos(-900,0);

    VoltGauge.setParentItem(&DrivingView);
    VoltGauge.setPos(0,0);
    vNeedle.setParentItem(&DrivingView);
    vNeedle.setPos(168,35);


    AmpGauge.setParentItem(&DrivingView);
    AmpGauge.setPos(0,240);
    aNeedle.setParentItem(&DrivingView);
    aNeedle.setPos(168,275);

    // motor
    Motor.setParentItem(&DrivingView);
    Motor.setTitle(tempDevice[tdMotor].Alias);
    Motor.setPos(620,10);

    // controller
    PwmC.setParentItem(&DrivingView);
    PwmC.setTitle(tempDevice[tdController].Alias);
    PwmC.setPos(620,100);

    // ambient
    FAmb.setParentItem(&DrivingView);
    FAmb.setTitle(tempDevice[tdFrontAmbient].Alias);
    FAmb.setPos(620,190);

    // 12V battery
    Bat12V.setParentItem(&DrivingView);
    Bat12V.setTitle("12V Battery");
    Bat12V.setPos(620,280);

    // bat charge view button
    BatBankViewButton->setParentItem(&DrivingView);
    BatBankViewButton->setPos(610,390);

    // shutdown button
    DVShutdownButton->setParentItem(&DrivingView);
    DVShutdownButton->setPos(700,390);

    // mute button
    DvMuteButton->setParentItem(&DrivingView);
    DvMuteButton->setPos(520,390);

    // wifi button
    WifiApButton->setParentItem(&DrivingView);
    WifiApButton->setPos(430,390);

    DrivingView.setData(0,QString("Display View"));
}

void CustomScene::LoadBatteryBankDisplay()
{
    addItem(&BatteryBankView);
    BatteryBankView.setPos(-900,0);


    for( int i=tdBatA; i<=tdBatL; i++ )
    {
        BatT[i].setParentItem(&BatteryBankView);
        BatT[i].setTitle(tempDevice[i].Alias);
    }


    // set position of items
    BatT[tdBatA].setPos(20,40);
    BatT[tdBatB].setPos(20,140);
    BatT[tdBatC].setPos(20,240);
    BatT[tdBatD].setPos(20,340);

    BatT[tdBatE].setPos(220,40);
    BatT[tdBatF].setPos(220,140);
    BatT[tdBatG].setPos(220,240);
    BatT[tdBatH].setPos(220,340);

    BatT[tdBatI].setPos(420,40);
    BatT[tdBatJ].setPos(420,140);
    BatT[tdBatK].setPos(420,240);
    BatT[tdBatL].setPos(420,340);

    BAmb.setParentItem(&BatteryBankView);
    BAmb.setTitle(tempDevice[tdBackAmbient].Alias);
    BAmb.setPos(600,50);

    // return button
    DriveViewButton->setParentItem(&BatteryBankView);
    DriveViewButton->setPos(700,300);

    // shutdown button
    BVShutdownButton->setParentItem(&BatteryBankView);
    BVShutdownButton->setPos(700,390);

    // mute button
    BvMuteButton->setParentItem(&BatteryBankView);
    BvMuteButton->setPos(700,210);

    BatteryBankView.setData(0,QString("Battery Bank View"));
}

void CustomScene::LoadTemperatureDisplay()
{
    addItem(&HighDefTempView);
    HighDefTempView.setPos(-900,0);

    ConfigButton->setParentItem(&HighDefTempView);
    ConfigButton->setPos(710,390);

    ReturnButton->setParentItem(&HighDefTempView);
    ReturnButton->setPos(10,390);

    HighDefTempView.setData(0,QString("Temperature View"));
}

void CustomScene::LoadAlarmConfigDisplay()
{
    addItem(&AlarmConfigView);
    AlarmConfigView.setPos(-900,0);

    A1Spinner.setValue(0);
    A2Spinner.setValue(0);
    A1Spinner.setPos(40,100);
    A2Spinner.setPos(340,100);

    A1Spinner.setParentItem(&AlarmConfigView);
    A2Spinner.setParentItem(&AlarmConfigView);

    AlarmConfigView.setData(0,QString("Alarm Configuration View"));

    TickButton->setParentItem(&AlarmConfigView);
    TickButton->setPos(710,390);
}

void CustomScene::LoadShutDownView()
{
    addItem(&ShutdownView);
    ShutdownView.setPos(-900,0);

    ShutDownButton->setParentItem(&ShutdownView);
    ShutDownButton->setPos(710,390);

    ShutdownReturnButton->setParentItem(&ShutdownView);
    ShutdownReturnButton->setPos(10,390);
}

void CustomScene::SetAlarmConfigDisplay(QString title, float a1, float a2)
{
    AlarmConfigView.SetTitle(title);
    A1Spinner.setValue(a1);
    A2Spinner.setValue(a2);
}

void CustomScene::VCAlarmSettingsChanged()
{
    if (DisplaySettingsDeviceIndex == tdBankVoltage)
    {
        powerDevice.Voltage.alarm1 = A1Spinner.getValue();
        powerDevice.Voltage.alarm2 = A2Spinner.getValue();
        SetConfiguration(DisplaySettingsDeviceIndex, powerDevice.Voltage.alarm1, powerDevice.Voltage.alarm2 );
    }
    else if (DisplaySettingsDeviceIndex == tdBankCurrent)
    {
        powerDevice.Current.alarm1 = A1Spinner.getValue();
        powerDevice.Current.alarm2 = A2Spinner.getValue();
        SetConfiguration(DisplaySettingsDeviceIndex, powerDevice.Current.alarm1 , powerDevice.Current.alarm2 );
    }
}

void CustomScene::TAlarmSettingsChanged()
{
    tempDevice[DisplaySettingsDeviceIndex].Alarm1 = A1Spinner.getValue();
    tempDevice[DisplaySettingsDeviceIndex].Alarm2 = A2Spinner.getValue();
    SetConfiguration(DisplaySettingsDeviceIndex, tempDevice[DisplaySettingsDeviceIndex].Alarm1, tempDevice[DisplaySettingsDeviceIndex].Alarm2);
}

void CustomScene::SetConfiguration(int device, float A1, float A2)
{
    QSettings settings(QString(S4WD_CONFIGURATION_FILE), QSettings::IniFormat);

    if ( device < 16 )
    {
        settings.beginGroup(QString("device%1").arg(device+1));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Device %1: Set Alarm1: %2").arg(device+1).arg( tempDevice[device].Alarm1);
        qDebug() << QString("Device %1: Set Alarm2: %2").arg(device+1).arg( tempDevice[device].Alarm2);
        settings.endGroup();
    }

    // bank voltage
    else if ( device == 17 )
    {
        settings.beginGroup(QString("bankvoltage"));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Bank battery V: Set Alarm1: %1").arg( powerDevice.Voltage.alarm1 );
        qDebug() << QString("Bank battery V: Set Alarm2: %1").arg( powerDevice.Voltage.alarm2 );
        settings.endGroup();
    }
    // bank current
    else if ( device == 18 )
    {
        settings.beginGroup(QString("bankcurrent"));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Bank battery C: Set Alarm1: %1").arg( powerDevice.Current.alarm1 );
        qDebug() << QString("Bank battery C: Set Alarm2: %1").arg( powerDevice.Current.alarm2 );
        settings.endGroup();
    }
}

void CustomScene::ChangeView(QGraphicsItem * view)
{
    qDebug() << "Hide: " << CurrentView->data(0).toString();
    CurrentView->setPos(-900,0);
    CurrentView = view;
    CurrentView->setPos(0,0);
    qDebug() << "Show: " << CurrentView->data(0).toString();
}

bool CustomScene::A1EqualA2()
{
    float A1 = A1Spinner.getValue();
    float A2 = A2Spinner.getValue();

    // voltage A1 > A2
    if (SpinUnits == suVoltage)
    {
        if (A1 > A2)
            return false;
        else
            return true;
    }
    // temperature and current A1 < A2
    else
    {
        if (A1 < A2)
            return false;
        else
            return true;
    }
}

void CustomScene::MutePress()
{
    if (MuteActive)
    {
        BvMuteButton->buttonStateChanged(false);
        DvMuteButton->buttonStateChanged(false);
        MuteActive = false;
        for (int i=0; i<=tdFrontAmbient; i++)
        {
            tempDevice[i].a1muted = false;
            tempDevice[i].a2muted = false;
        }

        powerDevice.Current.a1muted = false;
        powerDevice.Current.a2muted = false;
        powerDevice.Voltage.a1muted = false;
        powerDevice.Voltage.a2muted = false;

    }
    else
    {
        BvMuteButton->buttonStateChanged(true);
        DvMuteButton->buttonStateChanged(true);
        MuteActive = true;
        UpdateMutedAlarmMap();
    }
}

void CustomScene::PowerOff()
{
     PowerOffProcess = new QProcess(this);

     QString shutdown = "shutdown";
     QStringList argslist = QStringList() << "-h" << "now";
     qDebug() << "Powering off.";
     PowerOffProcess->start(shutdown,argslist);
}

void CustomScene::WifiPress()
{
    if (WifiActive)
    {
        WifiActive = false;
        WifiApButton->buttonStateChanged(false);
        WifiTurnOff();
    }
    else
    {
        WifiActive = true;
        WifiApButton->buttonStateChanged(true);
        WifiTurnOn();
    }
}

void CustomScene::WifiTurnOn()
{
#ifdef Q_OS_WIN
    qDebug() << "Turn on wifi.";
#else
    QString ifup = "./wifiapp";
    QStringList argslist = QStringList() << "on";
    qDebug() << "Turning wifi on.";
    wifiApOn.start(ifup,argslist);
#endif
}

void CustomScene::WifiTurnOff()
{
#ifdef Q_OS_WIN
    qDebug() << "Turn off wifi.";
#else

    QString ifup = "./wifiapp";
    QStringList argslist = QStringList() << "off";
    qDebug() << "Turning wifi off.";
    wifiApOn.start(ifup,argslist);
#endif
}

void CustomScene::SlotSpinnerDownTimerTimeout()
{
    SpinnerHoldDownTimer.stop();

    if (SpinnerPress == spNone)
        return;

    SpinnerHoldDownActiveTimer.start(SPINNER_HOLD_PRESS_TIMEOUT);

}

void CustomScene::SlotSpinnerDownActiveTimerTimeout()
{
    switch( SpinnerPress )
    {

    case spA1down:
        A1Spinner.changeValue(false, true, A2Spinner.getValue());
        break;

    case spA1up:
        A1Spinner.changeValue(true, true, A2Spinner.getValue());
        break;

    case spA2down:
        A2Spinner.changeValue(false, true, A1Spinner.getValue());
        break;

    case spA2up:
        A2Spinner.changeValue(true, true, A1Spinner.getValue());
        break;

    default:
        SpinnerHoldDownActiveTimer.stop();
        return;

    }
}




void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->accept();
    QPointF mousePoint = mouseEvent->buttonDownScenePos(Qt::LeftButton);
    //qDebug() << QString("Down x: %1 y: %2").arg(mousePoint.x()).arg( mousePoint.y());

    if ( TickButton->contains(TickButton->mapFromScene( mousePoint)) )
    {
        TickButton->buttonStateChanged(true);
        qDebug() << "tick press";
        return;
    }

    if ( BatBankViewButton->contains(BatBankViewButton->mapFromScene( mousePoint )) )
    {
        BatBankViewButton->buttonStateChanged(true);
        qDebug() << "bat view press";
        return;
    }

    if ( BVShutdownButton->contains(BVShutdownButton->mapFromScene( mousePoint )) )
    {
        BVShutdownButton->buttonStateChanged(true);
        qDebug() << "battery view shutdown button press";
        return;
    }

    if ( DVShutdownButton->contains(DVShutdownButton->mapFromScene( mousePoint )) )
    {
        DVShutdownButton->buttonStateChanged(true);
        qDebug() << "drive view shutdown button press";
        return;
    }


    if ( ShutdownReturnButton->contains(ShutdownReturnButton->mapFromScene( mousePoint )) )
    {
        ShutdownReturnButton->buttonStateChanged(true);
        qDebug() << "shutdown view return button press";
        return;
    }

    if ( ShutDownButton->contains(ShutDownButton->mapFromScene( mousePoint )) )
    {
        ShutDownButton->buttonStateChanged(true);
        qDebug() << "shutdown view shutdown button press";
        return;
    }

    if ( ReturnButton->contains(ReturnButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "return press";
        ReturnButton->buttonStateChanged(true);
        return;
    }

    if ( ConfigButton->contains(ConfigButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "config press";
        ConfigButton->buttonStateChanged(true);
        return;
    }

    if ( DriveViewButton->contains(DriveViewButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "drive view press";
        DriveViewButton->buttonStateChanged(true);
        return;
    }

    if ( BvMuteButton->contains(BvMuteButton->mapFromScene(mousePoint)) )
    {
        qDebug() << "bat view mute press";
        return;
    }


    if ( DvMuteButton->contains(BvMuteButton->mapFromScene(mousePoint)) )
    {
        qDebug() << "drive view mute press";
        return;
    }

    if (WifiApButton->contains(WifiApButton->mapFromScene(mousePoint)))
    {
        qDebug() << "wifi press";
        return;
    }

    if ( A1Spinner.spinnerAccess( mousePoint ) == spUpArrow )
    {
        qDebug() << "A1 up spinner press";
        A1Spinner.upArrowState(bsPressed);
        SpinnerPress = spA1up;
        SpinnerHoldDownTimer.start(SPINNER_HOLD_PRESS_TRIGGER);
        return;
    }

    if ( A1Spinner.spinnerAccess( mousePoint ) == spDownArrow )
    {
        qDebug() << "A1 down spinner press";
        A1Spinner.downArrowState(bsPressed);
        SpinnerPress = spA1down;
        SpinnerHoldDownTimer.start(SPINNER_HOLD_PRESS_TRIGGER);
        return;
    }

    if ( A2Spinner.spinnerAccess( mousePoint ) == spUpArrow )
    {
        qDebug() << "A2 up spinner press";
        A2Spinner.upArrowState(bsPressed);
        SpinnerPress = spA2up;
        SpinnerHoldDownTimer.start(SPINNER_HOLD_PRESS_TRIGGER);
        return;
    }

    if ( A2Spinner.spinnerAccess( mousePoint ) == spDownArrow )
    {
        qDebug() << "A2 down spinner press";
        A2Spinner.downArrowState(bsPressed);
        SpinnerPress = spA2down;
        SpinnerHoldDownTimer.start(SPINNER_HOLD_PRESS_TRIGGER);
        return;
    }

   // QGraphicsScene::mousePressEvent(mouseEvent);
}

void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    int i;
    QStringList titles;
    QPointF mousePoint = mouseEvent->buttonDownScenePos(Qt::LeftButton);

    mouseEvent->accept();
    //qDebug() << QString("Up x: %1 y: %2").arg(mousePoint.x()).arg( mousePoint.y());

    for (i=tdBatA; i<=tdBatL; i++)
    {
        // battery
        if ( BatT[i].contains(BatT[i].mapFromScene( mousePoint)) )
        {
            ReturnView = &BatteryBankView;
            DisplaySettingsDeviceIndex = i;
            titles.clear();
            titles << tempDevice[i].Alias;
            titles << tempDevice[i].SensorP.Alias;
            titles << tempDevice[i].SensorN.Alias;

            HighDefTempView.SetTitles(titles);
            HighDefTempView.SetValues( tempDevice[i].SensorP.T,
                                       tempDevice[i].SensorN.T,
                                       tempDevice[i].Alarm1,
                                       tempDevice[i].Alarm2);

            ChangeView(&HighDefTempView);
            return;
        }
    }

    // motor
    if ( Motor.contains(Motor.mapFromScene( mousePoint )) )
    {
        ReturnView = &DrivingView;
        DisplaySettingsDeviceIndex = tdMotor;
        titles.clear();
        titles << tempDevice[tdMotor].Alias;
        titles << tempDevice[tdMotor].SensorP.Alias;
        titles << tempDevice[tdMotor].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( tempDevice[tdMotor].SensorP.T,
                                   tempDevice[tdMotor].SensorN.T,
                                   tempDevice[tdMotor].Alarm1,
                                   tempDevice[tdMotor].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // controller
    else if ( PwmC.contains(PwmC.mapFromScene( mousePoint )) )
    {
        ReturnView = &DrivingView;
        DisplaySettingsDeviceIndex = tdController;
        titles.clear();
        titles << tempDevice[tdController].Alias;
        titles << tempDevice[tdController].SensorP.Alias;
        titles << tempDevice[tdController].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( tempDevice[tdController].SensorP.T,
                                   tempDevice[tdController].SensorN.T,
                                   tempDevice[tdController].Alarm1,
                                   tempDevice[tdController].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // front ambient
    else if ( FAmb.contains(FAmb.mapFromScene( mousePoint )) )
    {
        ReturnView = &DrivingView;
        DisplaySettingsDeviceIndex = tdFrontAmbient;
        titles.clear();
        titles << tempDevice[tdFrontAmbient].Alias;
        titles << tempDevice[tdFrontAmbient].SensorP.Alias;
        titles << tempDevice[tdFrontAmbient].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( tempDevice[tdFrontAmbient].SensorP.T,
                                   tempDevice[tdFrontAmbient].SensorN.T,
                                   tempDevice[tdFrontAmbient].Alarm1,
                                   tempDevice[tdFrontAmbient].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // back ambient
    else if ( BAmb.contains(BAmb.mapFromScene( mousePoint )) )
    {
        ReturnView = &BatteryBankView;
        DisplaySettingsDeviceIndex = tdBackAmbient;
        titles.clear();
        titles << tempDevice[tdBackAmbient].Alias;
        titles << tempDevice[tdBackAmbient].SensorP.Alias;
        titles << tempDevice[tdBackAmbient].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( tempDevice[tdBackAmbient].SensorP.T,
                                   tempDevice[tdBackAmbient].SensorN.T,
                                   tempDevice[tdBackAmbient].Alarm1,
                                   tempDevice[tdBackAmbient].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    if ( TickButton->contains(TickButton->mapFromScene( mousePoint)) )
    {
        qDebug() << "config view tick button release";
        TickButton->buttonStateChanged(false);

        if ( DisplaySettingsDeviceIndex < tdRunBatVoltage )
        {
            TAlarmSettingsChanged();
            titles.clear();
            titles << tempDevice[DisplaySettingsDeviceIndex].Alias;
            titles << tempDevice[DisplaySettingsDeviceIndex].SensorP.Alias;
            titles << tempDevice[DisplaySettingsDeviceIndex].SensorN.Alias;

            HighDefTempView.SetTitles(titles);
            HighDefTempView.SetValues( tempDevice[DisplaySettingsDeviceIndex].SensorP.T,
                                       tempDevice[DisplaySettingsDeviceIndex].SensorN.T,
                                       tempDevice[DisplaySettingsDeviceIndex].Alarm1,
                                       tempDevice[DisplaySettingsDeviceIndex].Alarm2);
            ChangeView(&HighDefTempView);
        }
        else
        {
            VCAlarmSettingsChanged();
            ChangeView(&DrivingView);
        }

        return;
    }

    if ( BatBankViewButton->contains(BatBankViewButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "driver view bat view button release";
        BatBankViewButton->buttonStateChanged(false);
        ChangeView(&BatteryBankView);
        return;
    }

    if ( DVShutdownButton->contains(DVShutdownButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "driver view shutdown button release";
        DVShutdownButton->buttonStateChanged(false);
        ReturnView = &DrivingView;
        ChangeView(&ShutdownView);
        return;
    }

    // mute in battery view
    if ( BvMuteButton->contains(BvMuteButton->mapFromScene(mousePoint)) )
    {
        qDebug() << "bat view mute release";
        MutePress();
        return;
    }

    if (WifiApButton->contains(WifiApButton->mapFromScene(mousePoint)))
    {
        qDebug() << "wifi release";
        WifiPress();
        return;
    }

    // mute in driver view
    if ( DvMuteButton->contains(DvMuteButton->mapFromScene(mousePoint)) )
    {
        qDebug() << "drive view mute press";
        MutePress();
        return;
    }

    if ( BVShutdownButton->contains(BVShutdownButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "bat view shutdown button release";
        BVShutdownButton->buttonStateChanged(false);
        ReturnView = &BatteryBankView;
        ChangeView(&ShutdownView);
        return;
    }

    if ( ShutdownReturnButton->contains(ShutdownReturnButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "shutdown view return button release";
        ShutdownReturnButton->buttonStateChanged(false);
        ChangeView(ReturnView);
        return;
    }

    if ( ShutDownButton->contains(ShutDownButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "shutdown view shutdown button release";
        ShutDownButton->buttonStateChanged(false);
        ShutdownView.ShuttingDown();
        PowerOff();
        return;
    }

    if ( ConfigButton->contains(ConfigButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "temp view config button release";
        ConfigButton->buttonStateChanged(false);
        AlarmConfigView.SetTitle( tempDevice[DisplaySettingsDeviceIndex].Alias );

        A1Spinner.setValue(tempDevice[DisplaySettingsDeviceIndex].Alarm1);
        A1Spinner.setParameters(suTemperature,0.5,-55.0,+125.0,stA1);

        A2Spinner.setValue(tempDevice[DisplaySettingsDeviceIndex].Alarm2);
        A2Spinner.setParameters(suTemperature,0.5,-55.0,+125.0,stA2);

        SpinUnits = suTemperature;
        ChangeView(&AlarmConfigView);
        return;
    }

    if ( ReturnButton->contains(ReturnButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "temp view config button release";
        ReturnButton->buttonStateChanged(false);
        ChangeView(ReturnView);
        return;
    }

    if ( DriveViewButton->contains(DriveViewButton->mapFromScene( mousePoint )) )
    {
        qDebug() << "bat view driver button release";
        DriveViewButton->buttonStateChanged(false);
        ChangeView(&DrivingView);
        return;
    }

    if ( A1Spinner.spinnerAccess( mousePoint ) == spUpArrow )
    {
        A1Spinner.changeValue(true, false, A2Spinner.getValue());
        if (SpinnerHoldDownActiveTimer.isActive())
            SpinnerHoldDownActiveTimer.stop();
        SpinnerPress = spNone;
        return;
    }

    if ( A1Spinner.spinnerAccess( mousePoint ) == spDownArrow )
    {
        A1Spinner.changeValue(false, false, A2Spinner.getValue());
        if (SpinnerHoldDownActiveTimer.isActive())
            SpinnerHoldDownActiveTimer.stop();
        SpinnerPress = spNone;
        return;
    }

    if ( A2Spinner.spinnerAccess( mousePoint ) == spUpArrow )
    {
        A2Spinner.changeValue(true, false, A1Spinner.getValue());
        if (SpinnerHoldDownActiveTimer.isActive())
            SpinnerHoldDownActiveTimer.stop();
        SpinnerPress = spNone;
        return;
    }

    if ( A2Spinner.spinnerAccess( mousePoint ) == spDownArrow )
    {
        A2Spinner.changeValue(false, false, A1Spinner.getValue());
        if (SpinnerHoldDownActiveTimer.isActive())
            SpinnerHoldDownActiveTimer.stop();
        SpinnerPress = spNone;
        return;
    }   




    // volts
    if ( VoltGauge.contains( VoltGauge.mapFromScene( mousePoint )))
    {
        AlarmConfigView.SetTitle(QString("Bank Voltage"));
        A1Spinner.setValue(powerDevice.Voltage.alarm1);
        A1Spinner.setParameters(suVoltage, (float)0.1, (float)40, (float)52, stA1);
        A2Spinner.setValue(powerDevice.Voltage.alarm2);
        A2Spinner.setParameters(suVoltage, (float)0.1, (float)40, (float)52, stA2);
        SpinUnits = suVoltage;
        DisplaySettingsDeviceIndex = tdBankVoltage;
        ReturnView = &DrivingView;
        ChangeView(&AlarmConfigView);
        return;
    }

    // amps
    if ( AmpGauge.contains( AmpGauge.mapFromScene( mousePoint )))
    {
        AlarmConfigView.SetTitle(QString("Bank Current"));
        A1Spinner.setValue(powerDevice.Current.alarm1);
        A1Spinner.setParameters(suCurrent,1, 0, 399, stA1);
        A2Spinner.setValue(powerDevice.Current.alarm2);
        SpinUnits = suCurrent;
        A2Spinner.setParameters(suCurrent,1, 0, 399, stA2);
        DisplaySettingsDeviceIndex = tdBankCurrent;
        ReturnView = &DrivingView;
        ChangeView(&AlarmConfigView);
        return;
    }

    //QGraphicsScene::mouseReleaseEvent(mouseEvent);

}

// called when button muted
void CustomScene::UpdateMutedAlarmMap()
{
    for (int i=0; i<=tdFrontAmbient; i++)
    {
        if (tempDevice[i].AlarmStatus == asAlarm1)
        {
            tempDevice[i].a1muted = true;
            tempDevice[i].a2muted = false;
        }
        else if (tempDevice[i].AlarmStatus == asAlarm2)
        {
            tempDevice[i].a1muted = false;
            tempDevice[i].a2muted = true;
        }
        else
        {
            tempDevice[i].a1muted = false;
            tempDevice[i].a2muted = false;
        }
    }

    if (powerDevice.Current.status == asAlarm1)
    {

        powerDevice.Current.a1muted = true;
        powerDevice.Current.a2muted = false;
    }
    else if (powerDevice.Current.status == asAlarm2)
    {
        powerDevice.Current.a1muted = false;
        powerDevice.Current.a2muted = true;
    }
    else
    {
        powerDevice.Current.a1muted = false;
        powerDevice.Current.a2muted = false;
    }

    if (powerDevice.Voltage.status == asAlarm1)
    {

        powerDevice.Voltage.a1muted = true;
        powerDevice.Voltage.a2muted = false;
    }
    else if (powerDevice.Voltage.status == asAlarm2)
    {
        powerDevice.Voltage.a1muted = false;
        powerDevice.Voltage.a2muted = true;
    }
    else
    {
        powerDevice.Voltage.a1muted = false;
        powerDevice.Voltage.a2muted = false;
    }

}


void CustomScene::SlotPowerNewReadings(QStringList readings)
{
    if (readings.count() != 2)
        return;

    //qDebug() << readings;


    float current = readings[0].toFloat();
    if (current >= 0 && current < 401)
        powerDevice.Current.reading = current;


    float voltage = readings[1].toFloat();
    if (voltage >= 42 && voltage < 61)
        powerDevice.Voltage.reading = voltage;

    DevicesUpdateAlarmStatus();
}

void CustomScene::SlotIOBat12VState(bool state)
{
    ioDevice.Bat12VGood = state;
    Bat12V.setState(state);
}

void CustomScene::SlotGpsNewData(QStringList newData)
{
    if (newData.count() != 7)
    {
        qDebug() << "Gps data missing.";
        return;
    }

    gpsData.latitude = newData[0];
    gpsData.longitude = newData[2];
    gpsData.speed = newData[4];
    gpsData.heading = newData[5];
    gpsData.altitude = newData[6];

    float speed = gpsData.speed.toFloat();

/*    if (speed == 0 && powerDevice.current.reading == 0)
    {
        if (!ChargeProfileActive)
        {
            emit SignalLoggingProfileChanged(true);
            DataCaptureTimer.start(CHARGE_PROFILE_DATA_CAPTURE_TIME);
        }

        ChargeProfileActive = true;
    }
    else
    {*/
        if (ChargeProfileActive)
        {
            emit SignalLoggingProfileChanged(false);
            DataCaptureTimer.start(DISCHARGE_PROFILE_DATA_CAPTURE_TIME);
        }

        ChargeProfileActive = false;
    //}
}



/* TIMERS */

void CustomScene::SlotFlashTimerTimeout()
{


    if (MuteActive)
    {
        SignalIOSirenOff(SIREN_TYPE_A1_PULSE);
        SignalIOSirenOff(SIREN_TYPE_A2_CONSTANT);
    }


    for (int i=tdBatA; i<=tdFrontAmbient; i++)
    {
        if ( tempDevice[i].AlarmStatus != asNone )
            DisplayFlashNow(i);
    }

    if ( powerDevice.Voltage.status != asNone )
        DisplayFlashNow(tdBankVoltage);

    if ( powerDevice.Current.status != asNone )
        DisplayFlashNow(tdBankCurrent);
}





void CustomScene::SlotLoggingTimerTimeout()
{
    emit SignalLoggingNewRecord(loggerData);
    loggerData.clear();
}

void CustomScene::SlotDataCaptureTimerTimeout()
{
    QDateTime dateTime = dateTime.currentDateTime();
    QString s;
    QString thisRecord;
    if (ChargeProfileActive)
    {
        QString timeNow = dateTime.toString("MM/dd HH:mm");
        thisRecord.append(timeNow);
        for (int i=0; i<=tdBackAmbient; i++)
            thisRecord.append( QString(",%1").arg(s.number( tempDevice[i].HighTemp, 'f', 1)) );

        thisRecord.append(QString(",%1").arg(s.number( tempDevice[tdFrontAmbient].HighTemp, 'f', 1)) );
        thisRecord.append(QString(",%1").arg(s.number( powerDevice.Voltage.reading, 'f', 1)) );

        loggerData << thisRecord;
    }
    else
    {
        QString timeNow = dateTime.toString("MM/dd HH:mm:ss");
        thisRecord.append(timeNow);
        thisRecord.append(QString(",%1").arg(s.number( tempDevice[tdMotor].HighTemp, 'f', 1)) );
        thisRecord.append(QString(",%1").arg(s.number( tempDevice[tdController].HighTemp, 'f', 1)) );
        thisRecord.append(QString(",%1").arg(s.number( powerDevice.Voltage.reading, 'f', 1)) );
        thisRecord.append(QString(",%1").arg(s.number( powerDevice.Current.reading, 'f', 1)) );
        thisRecord.append(QString(",%1").arg(gpsData.altitude));
        thisRecord.append(QString(",%1").arg(gpsData.heading));
        thisRecord.append(QString(",%1").arg(gpsData.speed));


        loggerData << thisRecord;
    }
}

void CustomScene::SlotBusInitialised(int threadno)
{
    switch(threadno)
    {
    case 1:
        qDebug() << QString("Reply: 1. Configure 2.");
        emit SignalBus2Configure(true);
        break;
    case 2:
        qDebug() << QString("Reply: 2. Configure 3.");
        emit SignalBus3Configure(true);
        break;
    case 3:
        qDebug() << QString("Reply: 3. Configure 4.");
        emit SignalBus4Configure(true);
        emit SignalGpsInitialise();
        break;
    default:
        qDebug() << QString("Unknown threadno: %1").arg(threadno);
    }
}



/* CONFIGURATION */
void CustomScene::GetConfiguration()
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
void CustomScene::SlotBus1NewTemps(QStringList newtemps)
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

void CustomScene::SlotBus2NewTemps(QStringList newtemps)
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

void CustomScene::SlotBus3NewTemps(QStringList newtemps)
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

void CustomScene::SlotBus4NewTemps(QStringList newtemps)
{

    //qDebug() << newtemps;

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


void CustomScene::DevicesUpdateAlarmStatus()
{
    bool A1Visual = false;
    bool A2Visual = false;
    bool A1Audio = false;
    bool A2Audio = false;

    //qDebug() << "Update alarm status";
    for (int i=tdBatA; i<=tdFrontAmbient; i++)
    {
        DisplaySetValue(i);

        // alarm status
        if ( tempDevice[i].HighTemp >= tempDevice[i].Alarm2 )
        {
            tempDevice[i].AlarmStatus = asAlarm2;
            DisplayStartFlashing(asAlarm2, i);
            A2Visual = true;


            if (!tempDevice[i].a2muted)
                A2Audio = true;
        }

        else if ( tempDevice[i].HighTemp >= tempDevice[i].Alarm1 )
        {
            tempDevice[i].AlarmStatus = asAlarm1;
            DisplayStartFlashing(asAlarm1, i);
            A1Visual = true;

            if (!tempDevice[i].a1muted && !tempDevice[i].a2muted)
                A1Audio = true;
        }

        else
        {
            DisplayStopFlashing(i);
            tempDevice[i].AlarmStatus = asNone;
        }
    }

    // bank voltage
    DisplaySetValue(tdBankVoltage);

    if ( powerDevice.Voltage.reading <=  powerDevice.Voltage.alarm2 )
    {
        powerDevice.Voltage.status = asAlarm2;
        DisplayStartFlashing(asAlarm2, tdBankVoltage);

        A2Visual = true;

        if (!powerDevice.Voltage.a2muted)
            A2Audio = true;
    }

    else if ( powerDevice.Voltage.reading <= powerDevice.Voltage.alarm1 )
    {
        powerDevice.Voltage.status = asAlarm1;
        DisplayStartFlashing(asAlarm1, tdBankVoltage);
        A1Visual = true;

        if (!powerDevice.Voltage.a1muted && !powerDevice.Voltage.a2muted)
            A1Audio = true;
    }

    else
    {
        DisplayStopFlashing(tdBankVoltage);
        powerDevice.Voltage.status = asNone;
    }

    // bank current
    DisplaySetValue(tdBankCurrent);
    if ( powerDevice.Current.reading >=  powerDevice.Current.alarm2 )
    {
        powerDevice.Current.status = asAlarm2;
        DisplayStartFlashing(asAlarm2, tdBankCurrent);
        A2Visual = true;

        if (!powerDevice.Current.a2muted)
            A2Audio = true;
    }

    else if ( powerDevice.Current.reading >= powerDevice.Current.alarm1 )
    {
        powerDevice.Current.status = asAlarm1;
        DisplayStartFlashing(asAlarm1, tdBankCurrent);
        A1Visual = true;
        if (!powerDevice.Current.a1muted && !powerDevice.Current.a2muted)
            A1Audio = true;
    }

    else
    {
        DisplayStopFlashing(tdBankCurrent);
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
    {
        SignalIOSirenOff(SIREN_TYPE_A1_PULSE);
        SignalIOSirenOn(SIREN_TYPE_A2_CONSTANT);
    }
    else if (A1Audio)
    {
        SignalIOSirenOff(SIREN_TYPE_A2_CONSTANT);
        SignalIOSirenOn(SIREN_TYPE_A1_PULSE);
    }
    else
    {
        SignalIOSirenOff(SIREN_TYPE_A1_PULSE);
        SignalIOSirenOff(SIREN_TYPE_A2_CONSTANT);
    }

}


void CustomScene::LoggerSetup()
{
    QObject::connect(&LoggingTimer, SIGNAL(timeout()), this, SLOT(SlotLoggingTimerTimeout()));



    Logger.moveToThread(&LoggerThread);
    QObject::connect(this, SIGNAL(SignalLoggingNewRecord(QStringList)), &Logger, SLOT(SlotNewRecord(QStringList)));
    QObject::connect(this, SIGNAL(SignalLoggingProfileChanged(bool)), &Logger, SLOT(SlotStartNewLogSession(bool)));
    LoggerThread.start();

    LoggingTimer.start(309000);
}

void CustomScene::PowerAgentSetup()
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

void CustomScene::IOAgentSetup()
{
    ioDevice.Bat12VMuted = false;
    ioDevice.Bat12VGood = true;


    InputOutput.moveToThread(&IOThread);
    QObject::connect(this, SIGNAL(SignalIOConfigure()), &InputOutput, SLOT(SlotConfigure()));
    QObject::connect(this, SIGNAL(SignalIOSirenOn(int)), &InputOutput, SLOT(SlotSirenOn(int)));
    QObject::connect(this, SIGNAL(SignalIOSirenOff(int)), &InputOutput, SLOT(SlotSirenOff(int)));

    QObject::connect(&InputOutput, SIGNAL(SignalBat12VState(bool)), this, SLOT(SlotIOBat12VState(bool)));



    IOThread.start();
    emit SignalIOConfigure();

}

void CustomScene::TemperatureSetup()
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
    QObject::connect(&Buses[0], SIGNAL(SignalCallerBusInitialised(int)),this, SLOT(SlotBusInitialised(int)));
    BusThread[0].start();




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
    QObject::connect(&Buses[1], SIGNAL(SignalCallerBusInitialised(int)),this, SLOT(SlotBusInitialised(int)));
    BusThread[1].start();




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
    QObject::connect(&Buses[2], SIGNAL(SignalCallerBusInitialised(int)),this, SLOT(SlotBusInitialised(int)));
    BusThread[2].start();



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
    QObject::connect(&Buses[3], SIGNAL(SignalCallerBusInitialised(int)),this, SLOT(SlotBusInitialised(int)));
    BusThread[3].start();



}

void CustomScene::GpsSetup()
{
    qDebug() << "Gps setup";
    Gps.moveToThread(&GpsThread);
    QObject::connect(&Gps, SIGNAL(SignalNewData(QStringList)), this, SLOT(SlotGpsNewData(QStringList)));
    QObject::connect(this, SIGNAL(SignalGpsInitialise()), &Gps, SLOT(SlotInitialise()));

    GpsThread.start();



}








































VoltGaugeItem::VoltGaugeItem()
{
    Volts = 46;
    flashOn = false;
    flash = new QGraphicsColorizeEffect;
    flash->setEnabled(false);
    setGraphicsEffect(flash);
}

VoltGaugeItem::~VoltGaugeItem()
{

}

QRectF VoltGaugeItem::boundingRect() const
{
    return QRectF(0,0,600,240);
}

void VoltGaugeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString s;

    QFont font;
    font.setFamily(QString("Consolas"));


    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(Qt::white));
    painter->setPen(pen);
    font.setPointSize(20);
    painter->setFont(font);
    painter->drawText( QPointF(380,60), QString("Volts"));
    font.setPointSize(40);
    painter->setFont(font);
    painter->drawText( QPointF(380,120), QString("%1V").arg(s.number( Volts, 'f', 1)));


    QColor red = QColor(254,58,19,255);
    QColor orange = QColor(255,255,0,255);
    QColor green = QColor(120,173,21,255);

    pen.setWidth(1);
    pen.setColor(QColor(Qt::black));
    painter->setPen(pen);

    // draw volt gauge
    QRectF rect;
    int startAngle;
    int spanAngle;

    // red overcharge area
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 0;
    spanAngle = 10*16;

    painter->setBrush(QBrush(red));
    painter->drawPie(rect,startAngle,spanAngle);

    // green
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 10*16;
    spanAngle = 110*16;
    painter->setBrush(QBrush(green));

    painter->drawPie(rect,startAngle,spanAngle);

    // amber
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 120*16;
    spanAngle = 30*16;
    painter->setBrush(QBrush(orange));
    painter->drawPie(rect,startAngle,spanAngle);

    // red discharge
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 150*16;
    spanAngle = 30*16;
    painter->setBrush(QBrush(red));
    painter->drawPie(rect,startAngle,spanAngle);


    // labels
    font.setPointSize(10);
    pen.setColor(QColor(Qt::black));
    painter->setPen(pen);
    painter->setFont(font);
    painter->drawText( QPointF(10,170), QString("42V"));
    painter->drawText( QPointF(40,95), QString("46V"));
    painter->drawText( QPointF(100,40), QString("48V"));
    painter->drawText( QPointF(315,145), QString("59V"));



}

void VoltGaugeItem::setValue(float volts)
{
    Volts = volts;

    update();
}

void VoltGaugeItem::flashNow()
{
    if (flashOn)
    {
        flashOn = false;
        flash->setStrength(1);
    }
    else
    {
        flashOn = true;
        flash->setStrength(0);
    }
    update(boundingRect());
}


void VoltGaugeItem::flashStart(TAlarmState AlarmEvent)
{
    if (AlarmEvent == asAlarm1)
        flash->setColor(QColor(255,255,0));

    else if (AlarmEvent == asAlarm2)
        flash->setColor(QColor(Qt::red));

    else if (AlarmEvent == asAlarmFastChanging)
        flash->setColor(QColor(Qt::blue));

    AlarmFlashEvent = AlarmEvent;
    flash->setEnabled(true);
}

void VoltGaugeItem::flashStop()
{
    flash->setEnabled(false);
    flash->setStrength(0);
    AlarmFlashEvent = asNone;
}

TAlarmState VoltGaugeItem::flashEvent()
{
    return AlarmFlashEvent;
}













CurrentGaugeItem::CurrentGaugeItem()
{
    Amps = 120;
    flashOn = false;
    flash = new QGraphicsColorizeEffect;
    flash->setEnabled(false);
    setGraphicsEffect(flash);


}

CurrentGaugeItem::~CurrentGaugeItem()
{
}

QRectF CurrentGaugeItem::boundingRect() const
{
    return QRectF(0,0,600,240);
}

void CurrentGaugeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString s;

    QFont font;
    font.setFamily(QString("Consolas"));

    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(Qt::white));
    painter->setPen(pen);
    font.setPointSize(20);
    painter->setFont(font);
    painter->drawText( QPointF(380,60), QString("Current"));
    font.setPointSize(40);
    painter->setFont(font);

    painter->drawText(QPointF(380,120), QString("%1A").arg(s.number( Amps, 'f', 0)));

    QColor red = QColor(254,58,19,255);
    QColor orange = QColor(255,255,0,255);
    QColor green = QColor(120,173,21,255);

    pen.setWidth(1);
    pen.setColor(QColor(Qt::black));
    painter->setPen(pen);

    // draw volt gauge
    QRectF rect;
    int startAngle;
    int spanAngle;

    // red overcharge area
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 0;
    spanAngle = 10*16;

    painter->setBrush(QBrush(red));
    painter->drawPie(rect,startAngle,spanAngle);

    // green
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 180*16;
    spanAngle = -90*16;
    painter->setBrush(QBrush(green));

    painter->drawPie(rect,startAngle,spanAngle);

    // amber
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 90*16;
    spanAngle = -45*16;
    painter->setBrush(QBrush(orange));
    painter->drawPie(rect,startAngle,spanAngle);

    // red discharge
    rect.setCoords(5.0,5.0,350.0,350.0);
    startAngle = 45*16;
    spanAngle = -45*16;
    painter->setBrush(QBrush(red));
    painter->drawPie(rect,startAngle,spanAngle);


    // labels
    font.setPointSize(10);
    pen.setColor(QColor(Qt::black));
    painter->setPen(pen);
    painter->setFont(font);
    painter->drawText( QPointF(10,170), QString("0A"));
    painter->drawText( QPointF(180,20), QString("200A"));
    painter->drawText( QPointF(260,60), QString("300A"));
    painter->drawText( QPointF(315,170), QString("400A"));
}

void CurrentGaugeItem::setValue(float amps)
{
    Amps = amps;
    update();
}

void CurrentGaugeItem::flashNow()
{
    if (flashOn)
    {
        flashOn = false;
        flash->setStrength(1);
    }
    else
    {
        flashOn = true;
        flash->setStrength(0);
    }
    update(boundingRect());
}


void CurrentGaugeItem::flashStart(TAlarmState AlarmEvent)
{
    if (AlarmEvent == asAlarm1)
        flash->setColor(QColor(255,255,0));

    else if (AlarmEvent == asAlarm2)
        flash->setColor(QColor(Qt::red));

    else if (AlarmEvent == asAlarmFastChanging)
        flash->setColor(QColor(Qt::blue));

    AlarmFlashEvent = AlarmEvent;
    flash->setEnabled(true);
}

void CurrentGaugeItem::flashStop()
{
    flash->setEnabled(false);
    flash->setStrength(0);
    AlarmFlashEvent = asNone;
}

TAlarmState CurrentGaugeItem::flashEvent()
{
    return AlarmFlashEvent;
}





AnalogNeedleItem::AnalogNeedleItem()
{
    Angle = -20;
}

AnalogNeedleItem::~AnalogNeedleItem()
{

}

QRectF AnalogNeedleItem::boundingRect() const
{
    return QRectF(0,0,20,150);
}

void AnalogNeedleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPainterPath path;
    path.moveTo(10.0,0.0);
    path.lineTo(0.0,130.0);
    path.arcTo(0.0,130.0,20.0,20.0,180,180);
    path.lineTo(10.0,0.0);
    QPen pen;
    pen.setColor(QColor(Qt::black));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(45,45,45,200)));
    painter->drawPath(path);

    pen.setColor(QColor(Qt::black));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(120,120,120,255)));

    painter->drawEllipse(5.0,135.0,10.0,10.0);
}

void AnalogNeedleItem::setValue(float value)
{
    Value = value;
    ValueToAngleTransformation();
    setTransformOriginPoint(10,140);
    setRotation(Angle);
}




VoltNeedle::VoltNeedle()
{
}

VoltNeedle::~VoltNeedle()
{

}

void VoltNeedle::ValueToAngleTransformation()
{
    Angle = -90 + (qreal)((Value-42) * 10);

    if ( Angle > 90 )
        Angle = 90;

    else if ( Angle < -90 )
        Angle = -90;
}




AmpNeedle::AmpNeedle()
{
}

AmpNeedle::~AmpNeedle()
{

}

void AmpNeedle::ValueToAngleTransformation()
{
    Angle = -90 + (qreal)(Value*0.45);

    if ( Angle > 90 )
        Angle = 90;

    else if (Angle < -90)
        Angle = -90;

}




ViewItem::ViewItem()
{
}

ViewItem::~ViewItem()
{
}

QRectF ViewItem::boundingRect() const
{
    return QRectF(0,0,800,480);
}

void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw panel
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);
    painter->setBrush(QColor(60,60,60,160));
    painter->drawPath(frame);
}






BatteryDisplayItem::BatteryDisplayItem()
{
    Fault = false;
    flashOn = false;
    Value = 0;
    Title = QString("");
    flash = new QGraphicsColorizeEffect;
    flash->setEnabled(false);
    setGraphicsEffect(flash);
}

BatteryDisplayItem::~BatteryDisplayItem()
{
    delete flash;
}

QRectF BatteryDisplayItem::boundingRect() const
{
    return QRectF(0,0,200,100);
}

void BatteryDisplayItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString s;

    QPixmap battery(PNG_IMAGE_BAT);
    painter->drawPixmap(0,0,160,100,battery);


    QPen pen;
    pen.setColor(QColor(Qt::black));
    pen.setWidth(2);
    painter->setPen(pen);

    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(15);
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(QPointF(10,40), Title);

    font.setPointSize(25);
    painter->setFont(font);


    if (Fault)
        painter->drawText( QPointF(10,90), QString("Fault") );
    else
        painter->drawText( QPointF(10,90), QString("%1%2C").arg(s.number( Value, 'f', 0)).arg(DEGREE));

}

void BatteryDisplayItem::setTitle(QString title)
{
    Title = title;
}


void BatteryDisplayItem::setValue(float value)
{
    Value = value;
    if (Value == 200.0)
        Fault = true;
    else
        Fault = false;
    //qDebug() << QString("Value %1").number(Value,'f',1);
    update(boundingRect());
}

void BatteryDisplayItem::flashNow()
{
    if (flashOn)
    {
        flashOn = false;
        flash->setStrength(1);
    }
    else
    {
        flashOn = true;
        flash->setStrength(0);
    }
    update(boundingRect());
}


void BatteryDisplayItem::flashStart(TAlarmState AlarmEvent)
{
    if (AlarmEvent == asAlarm1)
        flash->setColor(QColor(255,255,0));

    else if (AlarmEvent == asAlarm2)
        flash->setColor(QColor(Qt::red));

    else if (AlarmEvent == asAlarmFastChanging)
        flash->setColor(QColor(Qt::blue));

    AlarmFlashEvent = AlarmEvent;
    flash->setEnabled(true);
}

void BatteryDisplayItem::flashStop()
{
    flash->setEnabled(false);
    flash->setStrength(0);
    AlarmFlashEvent = asNone;
}

TAlarmState BatteryDisplayItem::flashEvent()
{
    return AlarmFlashEvent;
}


























DigitalDisplayItem::DigitalDisplayItem()
{
    Fault = false;
    flashOn = false;
    Value = 0;
    Title = QString("");
    flash = new QGraphicsColorizeEffect;
    flash->setEnabled(false);
    setGraphicsEffect(flash);
    setAcceptedMouseButtons(Qt::LeftButton);
}

DigitalDisplayItem::~DigitalDisplayItem()
{
    delete flash;
}

QRectF DigitalDisplayItem::boundingRect() const
{
    return QRectF(0,0,160,80);
}

void DigitalDisplayItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString s;

    QPen pen;
    pen.setColor(QColor(40,40,40));
    pen.setWidth(2);
    painter->setPen(pen);

    QPainterPath panel;
    panel.addRoundedRect(boundingRect(),4,4);
    painter->setBrush(QBrush(QColor(60,60,60,160)));
    painter->drawPath(panel);

    pen.setColor(QColor(Qt::white));
    pen.setWidth(2);

    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(15);
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(QPointF(5,21), Title);

    font.setPointSize(25);
    painter->setFont(font);

    if (Fault)
        painter->drawText( QPointF(5,70), QString("FAULT"));
    else
        painter->drawText( QPointF(5,70), QString("%1%2C").arg(s.number( Value, 'f', 0)).arg(DEGREE));


}

void DigitalDisplayItem::setTitle(QString title)
{
    Title = title;
}


void DigitalDisplayItem::setValue(float value)
{
    Value = value;
    if (Value == 200.0)
        Fault = true;
    else
        Fault = false;
    //qDebug() << QString("Value %1").number(Value,'f',1);
    update(boundingRect());
}

void DigitalDisplayItem::flashNow()
{
    if (flashOn)
    {
        flashOn = false;
        flash->setStrength(1);
    }
    else
    {
        flashOn = true;
        flash->setStrength(0);
    }
    update(boundingRect());
}


void DigitalDisplayItem::flashStart(TAlarmState AlarmEvent)
{
    if (AlarmEvent == asAlarm1) 
        flash->setColor(QColor(255,255,0));

    else if (AlarmEvent == asAlarm2)
        flash->setColor(QColor(Qt::red));

    else if (AlarmEvent == asAlarmFastChanging)
        flash->setColor(QColor(Qt::blue));

    AlarmFlashEvent = AlarmEvent;
    flash->setEnabled(true);
}

void DigitalDisplayItem::flashStop()
{
    flash->setEnabled(false);
    flash->setStrength(0);
    AlarmFlashEvent = asNone;
}

TAlarmState DigitalDisplayItem::flashEvent()
{
    return AlarmFlashEvent;
}



















ButtonItem::ButtonItem(QPixmap icon)
{
    ButtonPressed = false;
    ButtonIcon = icon;
}

ButtonItem::~ButtonItem()
{

}

QRectF ButtonItem::boundingRect() const
{
    return QRectF(0,0,80,80);
}

void ButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw frame
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);

    QPen pen;
    pen.setColor(QColor(40,40,40));
    pen.setWidth(2);
    painter->setPen(pen);

    if (ButtonPressed)
    {
        painter->setBrush(QColor(20,20,20,200));
        painter->drawPath(frame);
        painter->drawPixmap(8,8,64,64,ButtonIcon);
    }
    else
    {
        painter->setBrush(QColor(50,50,50,200));
        painter->drawPath(frame);
        painter->drawPixmap(8,8,64,64,ButtonIcon);
    }
}

void ButtonItem::buttonStateChanged(bool pressed)
{
    ButtonPressed = pressed;
    update();
}






















TemperatureView::TemperatureView()
{
    Title = QString("");
    Sensor1Alias = QString("");
    Sensor2Alias = QString("");
    Temperature1 = 0;
    Temperature2 = 0;
    Alarm1 = 0;
    Alarm2 = 0;
}

TemperatureView::~TemperatureView()
{

}

QRectF TemperatureView::boundingRect() const
{
    return QRectF(0,0,800,480);
}

void TemperatureView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);
    painter->setBrush(QColor(60,60,60,160));
    painter->drawPath(frame);

    QString s;
    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(40);

    QPen pen;
    pen.setColor(Qt::yellow);

    painter->setFont(font);
    painter->setPen(pen);

    // draw text items
    painter->drawText( QPointF(10,70), Title);

    font.setPointSize(20);
    painter->setFont(font);

    painter->drawText( QPointF(10,130), Sensor1Alias);
    painter->drawText( QPointF(400,130), Sensor2Alias);

    pen.setColor(Qt::white);
    painter->setPen(pen);

    font.setPointSize(60);
    painter->setFont(font);

    painter->drawText( QPointF(10,220), QString("%1%2C").arg(s.number( Temperature1, 'f', 1)).arg(DEGREE));
    painter->drawText( QPointF(400,220), QString("%1%2C").arg(s.number( Temperature2, 'f', 1)).arg(DEGREE));

    font.setPointSize(30);
    painter->setFont(font);
    painter->drawText( QPointF(10,300), QString("Alarm 1: %1%2C").arg(s.number( Alarm1, 'f', 1)).arg(DEGREE));
    painter->drawText( QPointF(10,340), QString("Alarm 2: %1%2C").arg(s.number( Alarm2, 'f', 1)).arg(DEGREE));
}

void TemperatureView::SetTitles(QStringList titles)
{
    Title = titles[0];
    Sensor1Alias = titles[1];
    Sensor2Alias = titles[2];
}

void TemperatureView::SetValues(float t1, float t2, float a1, float a2)
{
    Temperature1 = t1;
    Temperature2 = t2;
    Alarm1 = a1;
    Alarm2 = a2;
}

void TemperatureView::UpdateView()
{
    update();
}










AlarmConfigViewItem::AlarmConfigViewItem()
{
    Title = QString("");
}

AlarmConfigViewItem::~AlarmConfigViewItem()
{

}

QRectF AlarmConfigViewItem::boundingRect() const
{
    return QRectF(0,0,800,480);
}

void AlarmConfigViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw panel
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);
    painter->setBrush(QColor(60,60,60,160));
    painter->drawPath(frame);

    QFont font;
    QPen pen;

    font.setFamily(QString("Consolas"));
    font.setPointSize(40);

    pen.setColor(QColor(Qt::yellow));

    painter->setFont(font);
    painter->setPen(pen);

    painter->drawText(QPointF(40,50), Title);

    pen.setColor(QColor(Qt::gray));
    font.setPointSize(20);
    painter->setFont(font);
    painter->setPen(pen);

    painter->drawText(QPointF(150,90), QString("A1"));
    painter->drawText(QPointF(450,90), QString("A2"));

}

void AlarmConfigViewItem::SetTitle(QString title)
{
    Title = title;
}













BatteryBankViewItem::BatteryBankViewItem()
{

}

BatteryBankViewItem::~BatteryBankViewItem()
{

}

QRectF BatteryBankViewItem::boundingRect() const
{
     return QRectF(0,0,800,480);
}

void BatteryBankViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw panel
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);
    painter->setBrush(QColor(60,60,60,160));
    painter->drawPath(frame);

    QFont font;
    QPen pen;

    font.setFamily(QString("Consolas"));
    font.setPointSize(14);

    pen.setColor(QColor(Qt::yellow));

    painter->setFont(font);
    painter->setPen(pen);

    painter->drawText(QPointF(20,30), QString("Front Bank"));
    painter->drawText(QPointF(220,30), QString("Rear Bank A"));
    painter->drawText(QPointF(420,30), QString("Rear Bank B"));

}


ShutdownScreen::ShutdownScreen()
{
    Shutdown = false;
}

ShutdownScreen::~ShutdownScreen()
{

}

QRectF ShutdownScreen::boundingRect() const
{
    return QRectF(0,0,800,480);
}

void ShutdownScreen::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw panel
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);
    painter->setBrush(QColor(60,60,60,160));
    painter->drawPath(frame);

    QFont font;
    QPen pen;

    font.setFamily(QString("Consolas"));
    font.setPointSize(40);

    pen.setColor(QColor(Qt::gray));

    painter->setFont(font);
    painter->setPen(pen);

    if (Shutdown)
        painter->drawText(QPointF(150,250), QString("Shutting down ..."));
    else
        painter->drawText(QPointF(280,250), QString("Shutdown"));

}

void ShutdownScreen::ShuttingDown()
{
    Shutdown = true;
    update();
}






ToggleButtonItem::ToggleButtonItem(QPixmap upicon, QPixmap downicon)
{
    ButtonPressed = false;
    UpIcon = upicon;
    DownIcon = downicon;
}

ToggleButtonItem::~ToggleButtonItem()
{

}

QRectF ToggleButtonItem::boundingRect() const
{
    return QRectF(0,0,80,80);
}

void ToggleButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw frame
    QPainterPath frame;
    frame.addRoundedRect(boundingRect(),5,5);

    QPen pen;
    pen.setColor(QColor(40,40,40));
    pen.setWidth(2);
    painter->setPen(pen);

    if (ButtonPressed)
    {
        painter->setBrush(QColor(20,20,20,200));
        painter->drawPath(frame);
        painter->drawPixmap(8,8,64,64,DownIcon);
    }
    else
    {
        painter->setBrush(QColor(50,50,50,200));
        painter->drawPath(frame);
        painter->drawPixmap(8,8,64,64,UpIcon);
    }
}

void ToggleButtonItem::buttonStateChanged(bool pressed)
{
    ButtonPressed = pressed;
    update();
}






Battery12VItem::Battery12VItem()
{
    BatTitle = QString("12V Battery");
    BatstateColour = new QGraphicsColorizeEffect;
    BatstateColour->setEnabled(false);
    setGraphicsEffect(BatstateColour);
}

Battery12VItem::~Battery12VItem()
{
    delete BatstateColour;
}

QRectF Battery12VItem::boundingRect() const
{
    return QRectF(0,0,200,100);
}

void Battery12VItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPixmap battery(PNG_IMAGE_BAT);
    painter->drawPixmap(0,0,160,100,battery);


    QPen pen;
    pen.setColor(QColor(Qt::black));
    pen.setWidth(2);
    painter->setPen(pen);

    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(15);
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(QPointF(10,40), BatTitle);

    font.setPointSize(25);
    painter->setFont(font);


}

void Battery12VItem::setState(bool state)
{
    if (state)
        BatstateColour->setEnabled(false);
    else
    {
        BatstateColour->setEnabled(true);
        BatstateColour->setColor(QColor(Qt::red));
    }
}

void Battery12VItem::setTitle(QString title)
{
    BatTitle = title;
}


