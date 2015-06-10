#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <QString>


enum TSpinnerAccess {spNoArrow, spUpArrow, spDownArrow};
enum TButtonState {bsPressed, bsReleased};
enum TAlarmState {asNone, asAlarm1, asAlarm2, asAlarmFastChanging};


struct TSensor {
    QString Serial;
    QString Alias;
    float T;
};

struct TempDevice {
    QString Alias;
    TSensor SensorP;
    TSensor SensorN;
    float Alarm1;
    float Alarm2;
    float HighTemp;
    TAlarmState AlarmStatus;
    bool a1muted;
    bool a2muted;
};

struct TBus {
    QString Address;
    bool BusConnected;
    int SensorCount;
};


struct VA {
    float reading;
    float alarm1;
    float alarm2;
    TAlarmState status;
    bool a1muted;
    bool a2muted;
};

struct PowerDevice {
    VA Voltage;
    VA Current;
};

struct IODevice {
    bool Bat12VGood;
    bool Bat12VMuted;
    bool ignition;
};

#endif // DATASTRUCTS_H

