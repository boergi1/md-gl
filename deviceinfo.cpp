#include "deviceinfo.h"

//DeviceInfo::DeviceInfo(QObject *parent) : QObject(parent)
//{

//}

DeviceInfo::DeviceInfo() {}

QString DeviceInfo::deviceName() const
{
    return _deviceName;
}

QString DeviceInfo::deviceStateStr() const
{
    return _deviceStateStr;
}

quint16 DeviceInfo::deviceType() const
{
    return _deviceType;
}

qint32 DeviceInfo::deviceIndex() const
{
    return _deviceIndex;
}

QString DeviceInfo::deviceIdentifier() const
{
    return _deviceIdentifier;
}

void DeviceInfo::setDeviceName(QString name)
{
    _deviceName = name;
    emit deviceNameChanged(_deviceName);
}

void DeviceInfo::setDeviceStateStr(QString state)
{
    _deviceStateStr = state;
    emit deviceStateStrChanged(_deviceStateStr);
}

void DeviceInfo::setDeviceType(quint16 type)
{
    _deviceType = type;
    emit deviceTypeChanged(_deviceType);
}

void DeviceInfo::setDeviceIndex(qint32 index)
{
    _deviceIndex = index;
    emit deviceIndexChanged(_deviceIndex);
}

void DeviceInfo::setDeviceIdentifier(QString identifier)
{
    _deviceIdentifier = identifier;
    emit deviceIdentifierChanged(_deviceIdentifier);
}
