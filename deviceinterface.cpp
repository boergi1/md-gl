#include "deviceinterface.h"

//DeviceScanner::DeviceScanner(QObject *parent) : QObject(parent)
//{

//}

DeviceInterface::DeviceInterface()
{
    //        _deviceTypes = QStringList() << "Central" << "Front" << "Rear";

    //        _deviceTypes.reserve(3);
    //        _deviceTypes.insert(DeviceType_t::CENTRAL, "Central");
    //        _deviceTypes.insert(DeviceType_t::FRONT, "Front");
    //        _deviceTypes.insert(DeviceType_t::REAR, "Rear");
    //        emit deviceTypesChanged();

}

DeviceInterface::~DeviceInterface()
{
    this->clearDevices();
}

QVariant DeviceInterface::devices()
{
    return QVariant::fromValue(_deviceList);
}

QVariant DeviceInterface::deviceInfo()
{
    return QVariant::fromValue(_deviceInfoList);
}

//QStringList DeviceInterface::deviceTypes() { return _deviceTypes; }

bool DeviceInterface::scanning() { return _scanning; }

void DeviceInterface::startSearch()
{
    _scanning = true;
    emit scanningChanged();
    if(_deviceList.size()) this->clearDevices();

    quint32 deviceIndex = 0;


    // Realsense cameras
    rs2::context rs2_context;

    for (auto&& rs2_dev : rs2_context.query_devices())
    {
        QString serial_num = QString::fromStdString(rs2_dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
        QString name = QString::fromStdString(rs2_dev.get_info(RS2_CAMERA_INFO_NAME));
        qDebug()<< "Realsense camera found: #" << serial_num;
        rs2_camera_info cam_info_usb = static_cast<rs2_camera_info>(9);
        if (rs2_dev.supports(cam_info_usb))
        {
            double usb_type = std::atof( rs2_dev.get_info(cam_info_usb) );
            if (usb_type < 3.0)
            {
                qWarning() << "(DeviceInterface) USB type below 3.0: " << serial_num;
               // continue;
            }
        }
        else
        {
            qWarning() << "(DeviceInterface) Couldn't read USB type: " << serial_num;
          //  continue;
        }

        auto deviceInformation = new DeviceInfo();
        deviceInformation->setDeviceIndex(deviceIndex);
        deviceInformation->setDeviceName(name);
        deviceInformation->setDeviceIdentifier(serial_num);
        deviceInformation->setDeviceType(DeviceInterface::DeviceType::Realsense); // deviceTypeToString(DeviceInterface::DeviceType::Realsense)


        //        deviceInformation->setDeviceState(deviceStateToString(DeviceBaseClass::DeviceState::FOUND));

        //        device->setDeviceIndex(deviceIndex);
        //        device->setDeviceType(DeviceInterface::DeviceType::REALSENSE);
        //        device->setDeviceState(DeviceBaseClass::DeviceState::FOUND);
        //        device->setDeviceName(name);
        //        device->setDeviceIdentifier(serial_num);

        auto device = new RealsenseDevice(deviceIndex, this, rs2_dev);
        device->moveToThread(&device->deviceThread);
        connect(&device->deviceThread, &QThread::finished, device, &QObject::deleteLater);


        connect(device, &DeviceBaseClass::mainStateChanged, this, &DeviceInterface::handleDeviceState, Qt::DirectConnection);


        connect(device, &DeviceBaseClass::streamProfilesDataChanged, deviceInformation, &DeviceInfo::updateStreamProfiles, Qt::DirectConnection);
        connect(device, &DeviceBaseClass::sensorOptionsDataChanged, deviceInformation, &DeviceInfo::updateSensorOptions, Qt::DirectConnection);

      //  connect(this, &DeviceInterface::invokeWriteStreamProfile, device, &DeviceBaseClass::writeDeviceStreamProfile, Qt::DirectConnection );


       // writeDeviceStreamProfile



        //            connect(this, &DeviceInterface::startDevice, device, &RealsenseDevice::startCapture);
        connect(device, &RealsenseDevice::frameReady, this, &DeviceInterface::handleFrames, Qt::DirectConnection);




        _deviceList.append(device);
        _deviceInfoList.append(deviceInformation);

        //        device->setDeviceIndex(deviceIndex);
        //        device->setDeviceType(DeviceInterface::DeviceType::REALSENSE);
        //        device->setDeviceName(name);
        // device->setDeviceIdentifier(serial_num);

        device->setMainState(DeviceBaseClass::DeviceState::FOUND);


        emit devicesChanged(deviceIndex++);
        emit deviceInfoChanged();
    }

    // Other cameras (new class derived from devicebase)
    // ...
    //

    _scanning = false;
    emit scanningChanged();
}

void DeviceInterface::handleFrames(quint32 camIndex, QString data)
{
    qDebug()<<"handleFrames slot";
}

void DeviceInterface::clearDevices()
{
    for (QObject* devObj : _deviceList)
    {
        auto dev = static_cast<DeviceBaseClass*>(devObj);
        qDebug()<<"Waiting for device thread #"<<dev->deviceIndex()<<"to quit";
        dev->deviceThread.quit();
        dev->deviceThread.wait();
    }
    _deviceList.clear();
    for (QObject* devInfoObj : _deviceInfoList)
    {
        devInfoObj->deleteLater();
    }
    _deviceInfoList.clear();
    emit devicesChanged(-1);
}
