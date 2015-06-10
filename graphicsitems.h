#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QGraphicsColorizeEffect>
#include <QGesture>
#include <QGestureEvent>
#include <QDebug>
#include <QSettings>
#include <QProcess>
#include <vector>

#include "datastructs.h"
#include "busagent.h"
#include "ioagent.h"
#include "poweragent.h"
#include "gpsagent.h"
#include "logger.h"


#define DEGREE                          QChar(0260)
#define SPINNER_HOLD_PRESS_TRIGGER      200
#define SPINNER_HOLD_PRESS_TIMEOUT      180

#define CHARGE_PROFILE_DATA_CAPTURE_TIME    120000
#define DISCHARGE_PROFILE_DATA_CAPTURE_TIME 4000


#define HIGHER_TEMP(a,b,c)     a = (b > c) ? b:c




#define PNG_IMAGE_UP                    ":/icons/up.png"
#define PNG_IMAGE_DOWN                  ":/icons/down.png"
#define PNG_IMAGE_MUTE_OFF              ":/icons/muteoff.png"
#define PNG_IMAGE_MUTE_ON               ":/icons/muteon.png"
#define PNG_IMAGE_TICK                  ":/icons/tick.png"
#define PNG_IMAGE_BATCHARGE             ":/icons/batcharge.png"
#define PNG_IMAGE_CONFIG                ":/icons/config.png"
#define PNG_IMAGE_RETURN                ":/icons/back.png"
#define PNG_IMAGE_GAUGE                 ":/icons/gauge.png"
#define PNG_IMAGE_SHUTDOWN              ":/icons/shutdown.png"
#define PNG_IMAGE_WIFI_ON               ":/icons/wifion.png"
#define PNG_IMAGE_WIFI_OFF              ":/icons/wifioff.png"
#define PNG_IMAGE_BAT                   ":/icons/bat.png"



enum TDisplayView {dvNotSetYet, dvDriver, dvBatteryBank, dvTemp, dvAlarmConfig};

enum TTempDisplay { tdBatA=0,
                    tdBatB=1,
                    tdBatC=2,
                    tdBatD=3,
                    tdBatE=4,
                    tdBatF=5,
                    tdBatG=6,
                    tdBatH=7,
                    tdBatI=8,
                    tdBatJ=9,
                    tdBatK=10,
                    tdBatL=11,
                    tdBackAmbient=12,
                    tdMotor=13,
                    tdController=14,
                    tdFrontAmbient=15,
                    tdRunBatVoltage=16,
                    tdBankVoltage=17,
                    tdBankCurrent=18

                    };

enum TSpinnerUnit {suTemperature, suCurrent, suVoltage};
enum TSpinnerPress {spNone, spA1up, spA1down, spA2up, spA2down};
enum TSpinnerType {stA1, stA2};




const QStringList busAddress = QStringList() << "/dev/ttyS2" << "/dev/ttyS3" << "/dev/ttyS4" << "/dev/ttyS5";


class BatteryDisplayItem : public QGraphicsItem
{
public:
    BatteryDisplayItem();
    ~BatteryDisplayItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTitle( QString title );
    void setValue( float value );
    void flashStart(TAlarmState AlarmEvent);
    void flashStop();
    void flashNow();
    TAlarmState flashEvent();
private:
    TAlarmState AlarmFlashEvent;
    bool flashOn;
    float Value;
    QString Title;
    bool Fault;
    QGraphicsColorizeEffect *flash;
};

class Battery12VItem : public QGraphicsItem
{
public:
    Battery12VItem();
    ~Battery12VItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setState( bool state );
    void setTitle(QString title);
private:
    QString BatTitle;
    QGraphicsColorizeEffect *BatstateColour;
};


class DigitalDisplayItem : public QGraphicsItem
{
public:
    DigitalDisplayItem();
    ~DigitalDisplayItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTitle( QString title );
    void setValue( float value );
    void flashStart(TAlarmState AlarmEvent);
    void flashStop();
    void flashNow();
    TAlarmState flashEvent();
private:
    TAlarmState AlarmFlashEvent;
    bool flashOn;
    float Value;
    bool Fault;
    QString Title;
    QGraphicsColorizeEffect *flash;
};


class AnalogNeedleItem : public QGraphicsItem
{
public:
    AnalogNeedleItem();
    ~AnalogNeedleItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setValue( float value );
    qreal Angle;
    float Value;
    virtual void ValueToAngleTransformation() = 0;

};

class VoltNeedle: public AnalogNeedleItem
{
public:
    VoltNeedle();
    ~VoltNeedle();
    void ValueToAngleTransformation();
};

class AmpNeedle: public AnalogNeedleItem
{
public:
    AmpNeedle();
    ~AmpNeedle();
    void ValueToAngleTransformation();
};

class VoltGaugeItem : public QGraphicsItem
{
public:
    VoltGaugeItem();
    ~VoltGaugeItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setValue(float volts );
    void flashStart(TAlarmState AlarmEvent);
    void flashStop();
    void flashNow();
    TAlarmState flashEvent();

private:

   float Volts;
   bool flashOn;
   TAlarmState AlarmFlashEvent;
   QGraphicsColorizeEffect *flash;
};

class CurrentGaugeItem : public QGraphicsItem
{
public:
    CurrentGaugeItem();
    ~CurrentGaugeItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setValue(float amps);
    void flashStart(TAlarmState AlarmEvent);
    void flashStop();
    void flashNow();
    TAlarmState flashEvent();

private:

   float Amps;
   bool flashOn;
   TAlarmState AlarmFlashEvent;
   QGraphicsColorizeEffect *flash;
};



class AlarmSpinnerItem : public QGraphicsObject
{
public:
    AlarmSpinnerItem();
    ~AlarmSpinnerItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    float getValue();
    void setValue(float value);
    void setParameters(TSpinnerUnit unit, float resolution, float low, float high, TSpinnerType spintype);
    void changeValue(bool increment, bool fastscroll, float adjValue);
    void upArrowState( TButtonState bs );
    void downArrowState( TButtonState bs );
    TSpinnerAccess spinnerAccess(QPointF mousepoint);

private:
    TSpinnerUnit Unit;
    float Resolution;
    float LowerBound;
    float UpperBound;
    float Value;
    TSpinnerType SpinType;
    TButtonState UpArrow;
    TButtonState DownArrow;

};


class ButtonItem: public QGraphicsItem
{
public:
    ButtonItem(QPixmap icon);
    ~ButtonItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void buttonStateChanged( bool pressed );
private:
    bool ButtonPressed;
    QPixmap ButtonIcon;
};

class ToggleButtonItem: public QGraphicsItem
{
public:
    ToggleButtonItem(QPixmap upicon, QPixmap downicon);
    ~ToggleButtonItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void buttonStateChanged( bool pressed );
private:
    bool ButtonPressed;
    QPixmap UpIcon;
    QPixmap DownIcon;
};

class ViewItem : public QGraphicsItem
{
public:
    ViewItem();
    ~ViewItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class TemperatureView : public QGraphicsItem
{
public:
    TemperatureView();
    ~TemperatureView();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void SetTitles( QStringList titles);
    void SetValues(float t1, float t2, float a1, float a2);
    void UpdateView();

private:
    QString Title;
    QString Sensor1Alias;
    QString Sensor2Alias;
    float Temperature1;
    float Temperature2;
    float Alarm1;
    float Alarm2;
};




class AlarmConfigViewItem : public QGraphicsItem
{
public:
    AlarmConfigViewItem();
    ~AlarmConfigViewItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void SetTitle( QString title);

private:
    QString Title;
};


class BatteryBankViewItem : public QGraphicsItem
{
public:
    BatteryBankViewItem();
    ~BatteryBankViewItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


class ShutdownScreen : public QGraphicsItem
{
public:
    ShutdownScreen();
    ~ShutdownScreen();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void ShuttingDown();

private:
    bool Shutdown;

};



class CustomScene : public QGraphicsScene
{
    Q_OBJECT

public:
    CustomScene( QObject *parent = 0);
    ~CustomScene();

    void DisplaySetValue(int display);
    void DisplayStartFlashing(TAlarmState aEvent, int display);
    void DisplayStopFlashing(int display);
    void DisplayFlashNow(int display);
    void SetView(TDisplayView view);
    bool MuteIsActive();

    void SetBat12VState(bool state);
    TAlarmState AlarmMapContainsDevice(qint16 devid);
    TDisplayView GetView();

protected:

    void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

    DigitalDisplayItem Motor;
    DigitalDisplayItem PwmC;
    DigitalDisplayItem FAmb;
    DigitalDisplayItem BAmb;
    BatteryDisplayItem BatT[12];
    Battery12VItem Bat12V;
    VoltGaugeItem VoltGauge;
    VoltNeedle vNeedle;
    CurrentGaugeItem AmpGauge;
    AmpNeedle aNeedle;

    AlarmSpinnerItem A1Spinner;
    AlarmSpinnerItem A2Spinner;


    ToggleButtonItem * DvMuteButton;
    ToggleButtonItem * BvMuteButton;
    ButtonItem * TickButton;
    ButtonItem * BatBankViewButton;
    ButtonItem * ConfigButton;
    ButtonItem * ReturnButton;
    ButtonItem * DriveViewButton;
    ButtonItem * ShutDownButton;
    ButtonItem * ShutdownReturnButton;
    ButtonItem * DVShutdownButton;
    ButtonItem * BVShutdownButton;
    ToggleButtonItem * WifiApButton;



    // views
    ViewItem DrivingView;
    BatteryBankViewItem BatteryBankView;
    AlarmConfigViewItem AlarmConfigView;

    TemperatureView HighDefTempView;
    ShutdownScreen ShutdownView;

private slots:

    void                SlotFlashTimerTimeout();
    void                SlotLoggingTimerTimeout();
    void                SlotDataCaptureTimerTimeout();
    void                SlotSpinnerDownTimerTimeout();
    void                SlotSpinnerDownActiveTimerTimeout();

public slots:
    // temperatures
    void                SlotBusInitialised(int threadno);
    void                SlotBus1NewTemps(QStringList newtemps);
    void                SlotBus2NewTemps(QStringList newtemps);
    void                SlotBus3NewTemps(QStringList newtemps);
    void                SlotBus4NewTemps(QStringList newtemps);

    // power
    void                SlotPowerNewReadings(QStringList readings);

    // IO
    void                SlotIOBat12VState(bool state);

    // Gps
    void                SlotGpsNewData(QStringList newData);

signals:
    // Gps
    void                SignalGpsInitialise();


    // power
    void                SignalPowerConfigure();

    // IO
    void                SignalIOConfigure();
    void                SignalIOBat12VState();
    void                SignalIOSirenOn(int alarmEvent);
    void                SignalIOSirenOff(int alarmEvent);

    // temperature
    void                SignalBus1Configure(bool);
    void                SignalBus2Configure(bool);
    void                SignalBus3Configure(bool);
    void                SignalBus4Configure(bool);

    // logging
    void                SignalLoggingNewRecord(QStringList record);
    void                SignalLoggingProfileChanged(bool charging);

private:

    TempDevice          tempDevice[16];
    IODevice            ioDevice;
    PowerDevice         powerDevice;
    GpsData             gpsData;

    BusAgent            Buses[4];
    PowerAgent          Power;
    IOAgent             InputOutput;
    LoggerAgent         Logger;
    GpsAgent            Gps;

    QThread             GpsThread;
    QThread             PowerThread;
    QThread             IOThread;
    QThread             BusThread[4];
    QThread             LoggerThread;


    QTimer              FlashTimer;
    QTimer              LoggingTimer;
    QTimer              DataCaptureTimer;



    QGraphicsItem * ReturnView;
    QGraphicsItem * CurrentView;
    QTimer SpinnerHoldDownTimer;
    QTimer SpinnerHoldDownActiveTimer;

    QProcess * PowerOffProcess;
    QProcess wifiApOn;

    TSpinnerUnit SpinUnits;
    TSpinnerPress SpinnerPress;


    bool MuteActive;
    bool WifiActive;
    bool ChargeProfileActive;
    QStringList loggerData;

    void GetConfiguration();
    void DevicesUpdateAlarmStatus();
    void LoggerSetup();
    void PowerAgentSetup();
    void IOAgentSetup();
    void TemperatureSetup();
    void GpsSetup();
    void UpdateMutedAlarmMap();
    int  DisplaySettingsDeviceIndex;
    void LoadDriverDisplay();
    void LoadBatteryBankDisplay();
    void LoadTemperatureDisplay();
    void LoadAlarmConfigDisplay();
    void LoadShutDownView();
    void SetAlarmConfigDisplay(QString title, float a1, float a2);
    void TAlarmSettingsChanged();
    void VCAlarmSettingsChanged();
    void SetConfiguration(int device, float A1, float A2);
    void ChangeView(QGraphicsItem *view);
    bool A1EqualA2();
    void MutePress();
    void PowerOff();
    void WifiPress();
    void WifiTurnOn();
    void WifiTurnOff();



};




#endif // GRAPHICSITEMS_H

