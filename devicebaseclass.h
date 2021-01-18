#ifndef DEVICEBASECLASS_H
#define DEVICEBASECLASS_H

#include <QObject>
#include <QThread>
#include <QDebug>

//#include "deviceinterface.h"

#include <QPoint>


class DeviceInterface;

class DeviceBaseClass : public QObject
{
    Q_OBJECT
public:
    DeviceBaseClass(int id, DeviceInterface* parent)
    {
        _parent = parent;
        _state = DeviceBaseClass::DeviceState::FOUND;
        _index = id;
    }
    enum DeviceState {
        FOUND = 0,
        UNINIT,
        INIT,
        RECORD,
        STOP
    };
    Q_ENUM(DeviceState)

    QThread deviceThread;
    DeviceInterface* _parent;

    qint16 mainState() { return _state; }
    quint32 deviceIndex() { return _index; }

    bool isActive() { return _active; }

    QStringList streamProfileNames() { return _streamProfileNames; }
    QStringList streamProfileSettings() { return _streamProfileSettings; }
    QStringList sensorOptionsSettings() { return _sensorOptionsSettings; }


    void setMainState(DeviceState state) { _state = state; emit mainStateChanged(_index, _state); }


    void setStreamProfileSettings(QStringList names) { _streamProfileSettings = names; }
    void setSensorOptionsSettings(QStringList names) { _sensorOptionsSettings = names; }

public slots:
    virtual void initialize() {};
    virtual void startCapture() {};
    virtual void uninitialize() {};
    virtual void updateSettings() {};

    virtual void writeDeviceStreamProfile(QString streamName, QVariantList values) {};
    virtual void setDeviceSensorOption() {};

signals:
    void mainStateChanged(int idx, DeviceState state);
    void deviceCapturingChanged();
    void deviceRecordingChanged();

    void deviceIdentifierChanged();

    void streamProfilesDataChanged(QStringList value_names, QMultiMap<QString, QVariantList>* data);
    void sensorOptionsDataChanged(QStringList value_names, QMultiMap<QString, QVariantList>* data);

private:
    DeviceState _state;
    quint32 _index;
    QString _identifier;
    quint16 _type;

    bool _active = false;

    QStringList _streamProfileNames;
    QStringList _streamProfileSettings;
    QStringList _sensorOptionsSettings;

    QMultiMap<QString, QList<QVariant>> *_totalStreamProfilesPtr;
    QMultiMap<QString, QList<QVariant>>  _uniqueStreamProfiles;
    QMap<QString, QList<QVariant>> *_totalSensorOptionsPtr;


};

#endif // DEVICEBASECLASS_H
