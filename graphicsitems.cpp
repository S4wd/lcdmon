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
    QPixmap uparrow(":/up.png");
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
    QPixmap downarrow(":/down.png");
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

    MuteActive = false;
    WifiActive = false;

    BvMuteButton = new ToggleButtonItem(QPixmap(":/muteoff.png"),QPixmap(":/muteon.png"));
    DvMuteButton = new ToggleButtonItem(QPixmap(":/muteoff.png"),QPixmap(":/muteon.png"));
    TickButton = new ButtonItem(QPixmap(":/tick.png"));
    BatBankViewButton = new ButtonItem(QPixmap(":/batcharge.png"));
    ConfigButton = new ButtonItem(QPixmap(":/config.png"));
    ReturnButton = new ButtonItem(QPixmap(":/back.png"));
    DriveViewButton = new ButtonItem(QPixmap(":/gauge.png"));
    ShutDownButton = new ButtonItem(QPixmap(":/shutdown.png"));
    DVShutdownButton = new ButtonItem(QPixmap(":/shutdown.png"));
    BVShutdownButton= new ButtonItem(QPixmap(":/shutdown.png"));
    ShutdownReturnButton = new ButtonItem(QPixmap(":/back.png"));
    WifiApButton = new ToggleButtonItem(QPixmap(":/wifioff.png"), QPixmap(":/wifion.png"));

    QObject::connect(&SpinnerHoldDownTimer, SIGNAL(timeout()), this, SLOT(SlotSpinnerDownTimerTimeout()));
    QObject::connect(&SpinnerHoldDownActiveTimer, SIGNAL(timeout()), this, SLOT(SlotSpinnerDownActiveTimerTimeout()));
}

CustomScene::~CustomScene()
{
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

void CustomScene::Initialise(TempDevice *devdata, IODevice *iodata, PowerDevice *powerdata)
{
    // devices data
    DevicesData = devdata;
    IOData = iodata;
    PowerData = powerdata;

    LoadDriverDisplay();
    LoadBatteryBankDisplay();
    LoadTemperatureDisplay();
    LoadAlarmConfigDisplay();
    LoadShutDownView();

    // default display (dummy setting)
    CurrentView = &DrivingView;
    ChangeView(&DrivingView);
}

void CustomScene::SetView(TDisplayView view)
{
    if (view == dvDriver)
        ChangeView(&DrivingView);
    else if (view == dvBatteryBank)
        ChangeView(&BatteryBankView);
}

bool CustomScene::MuteIsActive()
{
    return MuteActive;
}

void CustomScene::SetBat12VState(bool state)
{
    Bat12V.setState(state);
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
        BatT[display].setValue(DevicesData[display].HighTemp);

    else if (display == tdMotor)
        Motor.setValue(DevicesData[tdMotor].HighTemp);

    else if (display == tdController)
        PwmC.setValue(DevicesData[tdController].HighTemp);

    else if (display == tdFrontAmbient)
        FAmb.setValue(DevicesData[tdFrontAmbient].HighTemp);

    else if (display == tdBackAmbient)
        BAmb.setValue(DevicesData[tdBackAmbient].HighTemp);



    else if (display == tdBankVoltage)
        VoltGauge.setValue(PowerData->Voltage.reading);

    else if (display == tdBankCurrent)
        AmpGauge.setValue(PowerData->Current.reading);

    if ( CurrentView == &HighDefTempView && display == DisplaySettingsDeviceIndex )
    {
        HighDefTempView.SetValues(DevicesData[display].SensorP.T,
                                  DevicesData[display].SensorN.T,
                                  DevicesData[display].Alarm1,
                                  DevicesData[display].Alarm2);
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

    AmpGauge.setParentItem(&DrivingView);
    AmpGauge.setPos(0,240);

    // motor
    Motor.setParentItem(&DrivingView);
    Motor.setTitle(DevicesData[tdMotor].Alias);
    Motor.setPos(620,10);

    // controller
    PwmC.setParentItem(&DrivingView);
    PwmC.setTitle(DevicesData[tdController].Alias);
    PwmC.setPos(620,100);

    // ambient
    FAmb.setParentItem(&DrivingView);
    FAmb.setTitle(DevicesData[tdFrontAmbient].Alias);
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

    // wifif button
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
        BatT[i].setTitle(DevicesData[i].Alias);
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
    BAmb.setTitle(DevicesData[tdBackAmbient].Alias);
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
        PowerData->Voltage.alarm1 = A1Spinner.getValue();
        PowerData->Voltage.alarm2 = A2Spinner.getValue();
        SetConfiguration(DisplaySettingsDeviceIndex, PowerData->Voltage.alarm1, PowerData->Voltage.alarm2 );
    }
    else if (DisplaySettingsDeviceIndex == tdBankCurrent)
    {
        PowerData->Current.alarm1 = A1Spinner.getValue();
        PowerData->Current.alarm2 = A2Spinner.getValue();
        SetConfiguration(DisplaySettingsDeviceIndex, PowerData->Current.alarm1 , PowerData->Current.alarm2 );
    }
}

void CustomScene::TAlarmSettingsChanged()
{
    DevicesData[DisplaySettingsDeviceIndex].Alarm1 = A1Spinner.getValue();
    DevicesData[DisplaySettingsDeviceIndex].Alarm2 = A2Spinner.getValue();
    SetConfiguration(DisplaySettingsDeviceIndex, DevicesData[DisplaySettingsDeviceIndex].Alarm1, DevicesData[DisplaySettingsDeviceIndex].Alarm2);
}

void CustomScene::SetConfiguration(int device, float A1, float A2)
{
    QSettings settings(QString(S4WD_CONFIGURATION_FILE), QSettings::IniFormat);

    if ( device < 16 )
    {
        settings.beginGroup(QString("device%1").arg(device+1));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Device %1: Set Alarm1: %2").arg(device+1).arg( DevicesData[device].Alarm1);
        qDebug() << QString("Device %1: Set Alarm2: %2").arg(device+1).arg( DevicesData[device].Alarm2);
        settings.endGroup();
    }

    // bank voltage
    else if ( device == 17 )
    {
        settings.beginGroup(QString("bankvoltage"));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Bank battery V: Set Alarm1: %1").arg( PowerData->Voltage.alarm1 );
        qDebug() << QString("Bank battery V: Set Alarm2: %1").arg( PowerData->Voltage.alarm2 );
        settings.endGroup();
    }
    // bank current
    else if ( device == 18 )
    {
        settings.beginGroup(QString("bankcurrent"));
        settings.setValue(QString("A1"), QVariant(A1).toString());
        settings.setValue(QString("A2"), QVariant(A2).toString());
        qDebug() << QString("Bank battery C: Set Alarm1: %1").arg( PowerData->Current.alarm1 );
        qDebug() << QString("Bank battery C: Set Alarm2: %1").arg( PowerData->Current.alarm2 );
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
            DevicesData[i].a1muted = false;
            DevicesData[i].a2muted = false;
        }

        PowerData->Current.a1muted = false;
        PowerData->Current.a2muted = false;
        PowerData->Voltage.a1muted = false;
        PowerData->Voltage.a2muted = false;

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
    QString ifup = "./wifiapp";
    QStringList argslist = QStringList() << "on";
    qDebug() << "Turning wifi on.";
    wifiApOn.start(ifup,argslist);
}

void CustomScene::WifiTurnOff()
{
    QString ifup = "./wifiapp";
    QStringList argslist = QStringList() << "off";
    qDebug() << "Turning wifi off.";
    wifiApOn.start(ifup,argslist);
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
            titles << DevicesData[i].Alias;
            titles << DevicesData[i].SensorP.Alias;
            titles << DevicesData[i].SensorN.Alias;

            HighDefTempView.SetTitles(titles);
            HighDefTempView.SetValues( DevicesData[i].SensorP.T,
                                       DevicesData[i].SensorN.T,
                                       DevicesData[i].Alarm1,
                                       DevicesData[i].Alarm2);

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
        titles << DevicesData[tdMotor].Alias;
        titles << DevicesData[tdMotor].SensorP.Alias;
        titles << DevicesData[tdMotor].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( DevicesData[tdMotor].SensorP.T,
                                   DevicesData[tdMotor].SensorN.T,
                                   DevicesData[tdMotor].Alarm1,
                                   DevicesData[tdMotor].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // controller
    else if ( PwmC.contains(PwmC.mapFromScene( mousePoint )) )
    {
        ReturnView = &DrivingView;
        DisplaySettingsDeviceIndex = tdController;
        titles.clear();
        titles << DevicesData[tdController].Alias;
        titles << DevicesData[tdController].SensorP.Alias;
        titles << DevicesData[tdController].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( DevicesData[tdController].SensorP.T,
                                   DevicesData[tdController].SensorN.T,
                                   DevicesData[tdController].Alarm1,
                                   DevicesData[tdController].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // front ambient
    else if ( FAmb.contains(FAmb.mapFromScene( mousePoint )) )
    {
        ReturnView = &DrivingView;
        DisplaySettingsDeviceIndex = tdFrontAmbient;
        titles.clear();
        titles << DevicesData[tdFrontAmbient].Alias;
        titles << DevicesData[tdFrontAmbient].SensorP.Alias;
        titles << DevicesData[tdFrontAmbient].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( DevicesData[tdFrontAmbient].SensorP.T,
                                   DevicesData[tdFrontAmbient].SensorN.T,
                                   DevicesData[tdFrontAmbient].Alarm1,
                                   DevicesData[tdFrontAmbient].Alarm2);

        ChangeView(&HighDefTempView);
        return;

    }

    // back ambient
    else if ( BAmb.contains(BAmb.mapFromScene( mousePoint )) )
    {
        ReturnView = &BatteryBankView;
        DisplaySettingsDeviceIndex = tdBackAmbient;
        titles.clear();
        titles << DevicesData[tdBackAmbient].Alias;
        titles << DevicesData[tdBackAmbient].SensorP.Alias;
        titles << DevicesData[tdBackAmbient].SensorN.Alias;

        HighDefTempView.SetTitles(titles);
        HighDefTempView.SetValues( DevicesData[tdBackAmbient].SensorP.T,
                                   DevicesData[tdBackAmbient].SensorN.T,
                                   DevicesData[tdBackAmbient].Alarm1,
                                   DevicesData[tdBackAmbient].Alarm2);

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
            titles << DevicesData[DisplaySettingsDeviceIndex].Alias;
            titles << DevicesData[DisplaySettingsDeviceIndex].SensorP.Alias;
            titles << DevicesData[DisplaySettingsDeviceIndex].SensorN.Alias;

            HighDefTempView.SetTitles(titles);
            HighDefTempView.SetValues( DevicesData[DisplaySettingsDeviceIndex].SensorP.T,
                                       DevicesData[DisplaySettingsDeviceIndex].SensorN.T,
                                       DevicesData[DisplaySettingsDeviceIndex].Alarm1,
                                       DevicesData[DisplaySettingsDeviceIndex].Alarm2);
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
        AlarmConfigView.SetTitle( DevicesData[DisplaySettingsDeviceIndex].Alias );

        A1Spinner.setValue(DevicesData[DisplaySettingsDeviceIndex].Alarm1);
        A1Spinner.setParameters(suTemperature,0.5,-55.0,+125.0,stA1);

        A2Spinner.setValue(DevicesData[DisplaySettingsDeviceIndex].Alarm2);
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
        A1Spinner.setValue(PowerData->Voltage.alarm1);
        A1Spinner.setParameters(suVoltage, (float)0.1, (float)40, (float)52, stA1);
        A2Spinner.setValue(PowerData->Voltage.alarm2);
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
        A1Spinner.setValue(PowerData->Current.alarm1);
        A1Spinner.setParameters(suCurrent,1, 0, 240, stA1);
        A2Spinner.setValue(PowerData->Current.alarm2);
        SpinUnits = suCurrent;
        A2Spinner.setParameters(suCurrent,1, 0, 240, stA2);
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
        if (DevicesData[i].AlarmStatus == asAlarm1)
            DevicesData[i].a1muted = true;

        else if (DevicesData[i].AlarmStatus == asAlarm2)
            DevicesData[i].a2muted = true;
    }
}


















VoltGaugeItem::VoltGaugeItem()
{
    Volts = 46;
    flashOn = false;
    flash = new QGraphicsColorizeEffect;
    flash->setEnabled(false);
    setGraphicsEffect(flash);

    vNeedle.setParentItem(this);
    vNeedle.setPos(155,38);
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


    // draw volt gauge
    QPixmap voltgauge(":/voltgauge.png");
    painter->drawPixmap(5,0,350,180,voltgauge);

}

void VoltGaugeItem::setValue(float volts)
{
    Volts = volts;
    vNeedle.setValue(volts);
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

    aNeedle.setParentItem(this);
    aNeedle.setPos(155,38);
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

    // draw amp gauge
    QPixmap ampgauge(":/ampgauge.png");
    painter->drawPixmap(5,0,350,180,ampgauge);
}

void CurrentGaugeItem::setValue(float amps)
{
    Amps = amps;
    aNeedle.setValue(amps);
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
    return QRectF(0,0,31,130);
}

void AnalogNeedleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPixmap needle(":/needle.png");
    painter->drawPixmap(0,0,51,160,needle);
}

void AnalogNeedleItem::setValue(float value)
{
    Value = value;
    ValueToAngleTransformation();
    setTransformOriginPoint(25,137);
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
    Angle = -90 + (qreal)((Value-40) * 15);

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
    Angle = -90 + (qreal)(Value*3/4);

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

    QPixmap battery(":/bat.png");
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
    BatstateColour->setEnabled(true);
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
    QString s;

    QPixmap battery(":/bat.png");
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
        BatstateColour->setColor(QColor(Qt::green));
    else
        BatstateColour->setColor(QColor(Qt::red));
}

void Battery12VItem::setTitle(QString title)
{
    BatTitle = title;
}


