#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDebug>


#include "deviceinfo.h"

#include <realsensedevice.h>

//#include <globaltypes.h>


class DeviceInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

    Q_PROPERTY(QVariant deviceInfo READ deviceInfo NOTIFY deviceInfoChanged)



    //    Q_PROPERTY(QStringList deviceTypes READ deviceTypes NOTIFY deviceTypesChanged)
public:
    DeviceInterface();

    ~DeviceInterface();

    //    enum DeviceState {
    //        FOUND = 0,
    //        UNINIT,
    //        INIT,
    //        RECORD,
    //        STOP
    //    };
    //    Q_ENUM(DeviceState)

    enum DeviceType {
        Other = 0,
        Realsense,
        UVC
    };
    Q_ENUM(DeviceType)

    enum DeviceUserType {
        None = 0,
        Central,
        Front,
        Rear
    };
    Q_ENUM(DeviceUserType)


    QVariant devices();

    QVariant deviceInfo();

    //    QStringList deviceTypes();

    bool scanning();



    QString deviceStateToString(DeviceBaseClass::DeviceState state)
    {
        switch (state)
        {
        case DeviceBaseClass::DeviceState::FOUND: return "Found";
        case DeviceBaseClass::DeviceState::UNINIT: return "Uninitialized";
        case DeviceBaseClass::DeviceState::INIT: return "Initialized";
        case DeviceBaseClass::DeviceState::RECORD: return "Recording";
        case DeviceBaseClass::DeviceState::STOP: return "Stopping";
        default: return "State";
        }
    }

    QString deviceTypeToString(DeviceType type)
    {
        switch (type)
        {
        case DeviceType::Other: return "Other";
        case DeviceType::Realsense: return "Realsense";
        case DeviceType::UVC: return "UVC";
        default: return "Type";
        }
    }

    QString deviceUserTypeToString(DeviceUserType usertype)
    {
        switch (usertype)
        {
        case DeviceUserType::None: return "None";
        case DeviceUserType::Central: return "Central";
        case DeviceUserType::Front: return "Front";
        case DeviceUserType::Rear: return "Rear";
        default: return "UserType";
        }
    }



    Q_INVOKABLE bool validateStreamProfile(int deviceIndex, QString streamName, QVariantList streamValues)
    {
        qDebug()<<"validateStreamProfile"<<deviceIndex<<streamName<<streamValues;
        auto deviceInfo = static_cast<DeviceInfo*>( _deviceInfoList.at(deviceIndex) );
        return deviceInfo->isValidStreamProfile(streamName, streamValues);
    }

    Q_INVOKABLE void setStreamProfile(int deviceIndex, QString streamName, QVariantList streamValues)
    {
        qDebug()<<"setStreamProfile"<<deviceIndex<<streamName<<streamValues;

        auto device = static_cast<DeviceBaseClass*>( _deviceList.at(deviceIndex) );
        auto deviceInfo = static_cast<DeviceInfo*>( _deviceInfoList.at(deviceIndex) );


        device->writeDeviceStreamProfile(streamName, streamValues);

    }

    Q_INVOKABLE QString rs2FormatToString(int deviceIndex, int rs2_format)
    {
        qDebug()<<"rs2FormatToString"<<rs2_format;
        auto device = static_cast<RealsenseDevice*>( _deviceList.at(deviceIndex) );
        return device->formatToString(rs2_format);
    }


    DeviceBaseClass::DeviceState deviceState() const
    {
        return m_deviceState;
    }

public slots:
    void startSearch();

    void initializeDevice(quint32 index)
    {
        qDebug()<<"initializeDevice"<<index;
        static_cast<DeviceBaseClass*>(_deviceList.at(index))->initialize();
    }


    void uninitializeDevice(quint32 index)
    {
        qDebug()<<"uninitializeDevice"<<index;
        static_cast<DeviceBaseClass*>(_deviceList.at(index))->uninitialize();
    }



    void handleFrames(quint32 camIndex, QString data);

    void handleDeviceState(int idx, DeviceBaseClass::DeviceState state)
    {
        qDebug()<<"handleDeviceState"<<idx<<state;
        emit invokeDeviceStateUpdate(idx, state);
        auto device = static_cast<DeviceBaseClass*>(_deviceList.at(idx));

        auto deviceInformation = static_cast<DeviceInfo*>(_deviceInfoList.at(idx));

        deviceInformation->setDeviceState(device->mainState());
        deviceInformation->setDeviceStateStr(deviceStateToString(static_cast<DeviceBaseClass::DeviceState>(device->mainState())));

        qDebug()<<"State of"<<deviceInformation->deviceType()<<"changed to:";



        switch (device->mainState())
        {
        case DeviceBaseClass::DeviceState::FOUND:
            qDebug()<<"Found";

            device->updateSettings();
            break;
        case DeviceBaseClass::DeviceState::UNINIT:
            qDebug()<<"Uninitialized";

            //     device->initialize();
            break;
            //        case DeviceBaseClass::DeviceState::UNINIT:
            //            qDebug()<<"Uninitialized";
            //            qWarning()<<"Initializing with default options";
            //            device->initialize();
            //            break;
        case DeviceBaseClass::DeviceState::INIT:
            qDebug()<<"Initialized";
            break;
        case DeviceBaseClass::DeviceState::RECORD:
            qDebug()<<"Recording";
            break;
        case DeviceBaseClass::DeviceState::STOP:
            qDebug()<<"Stopping";
            break;
        default: break;
        }


    }




    //        std::vector<CameraType_t> DeviceInterface::connectRealSenseDevices()
    //        {
    //            size_t device_id = 0;
    //            std::vector<CameraType_t> devices;

    //            for (auto&& dev : m_ctx.query_devices())
    //            {
    //                CameraType_t cam_id;
    //                std::string serial_num = dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

    //                // test usb connection
    //                rs2_camera_info cam_info_usb = static_cast<rs2_camera_info>(9);
    //                if (dev.supports(cam_info_usb))
    //                {
    //                    double usb_type = std::atof( dev.get_info(cam_info_usb) );
    //                    if (usb_type < 3.0)
    //                    {
    //                        std::cerr << "(DeviceInterface) USB type below 3.0: " << serial_num << std::endl;
    //                        continue;
    //                    }
    //                }
    //                else
    //                {
    //                    std::cerr << "(DeviceInterface) Couldn't read USB type: " << serial_num << std::endl;
    //                    continue;
    //                }

    //                // get position by serial number
    //                if (serial_num == RS_CENTRAL_SERIAL)
    //                    cam_id = CameraType_t::CENTRAL;
    //                else if (serial_num == RS_FRONT_SERIAL)
    //                    cam_id = CameraType_t::FRONT;
    //                else if (serial_num == RS_REAR_SERIAL)
    //                    cam_id = CameraType_t::REAR;
    //                else
    //                {
    //                    std::cerr << "(DeviceInterface) No matching RS2 serial number: " << serial_num << std::endl;
    //                    // pos_id = CamPosition_t::OTHER;
    //                    continue;
    //                }

    //                m_depth_frame_queues.push_back(new FrameQueue(cam_id, "depth"));
    //        #if RS_COLOR_ENABLED
    //                m_color_frame_queues.push_back(new FrameQueue(cam_id, "color"));
    //                m_rs2_devices.push_back(new Rs2Device( dev, device_id, cam_id, m_depth_frame_queues.back(), m_color_frame_queues.back()));
    //        #else
    //                m_rs2_devices.push_back(new Rs2Device( dev, device_id, cam_id, m_depth_frame_queues.back()));
    //        #endif
    //                m_rs2_devices.back()->setCaptureEnabled(true);
    //                devices.push_back(cam_id);
    //                device_id++;
    //                std::this_thread::sleep_for(std::chrono::seconds(3));
    //            }

    //            std::vector<Rs2Device*>::iterator iter = m_rs2_devices.begin();
    //            while( iter != m_rs2_devices.end() )
    //            {
    //                if ( ! (*iter)->isActive() )
    //                {
    //                    std::cerr << "(DeviceInterface) Rs2Device not active: " << (*iter)->getCamTypeStr() << std::endl;
    //                    device_id--;
    //                    for (size_t i = 0; i < devices.size(); i++) {
    //                        if (devices.at(i) == (*iter)->getCamType())
    //                        {
    //                            devices.erase(devices.begin() + i);
    //                            break;
    //                        }
    //                    }
    //                }
    //                ++iter;
    //            }

    //            return devices;

    //        }

signals:
    void devicesChanged(int index);
    void deviceInfoChanged();
    void scanningChanged();
    //    void deviceTypesChanged();

    void invokeWriteStreamProfile(QString streamName, QVariantList values);

    void invokeDeviceStateUpdate(int idx, DeviceBaseClass::DeviceState state);

    void deviceStateChanged(DeviceBaseClass::DeviceState deviceState);

private:
    QList<QObject*> _deviceList;
    bool _scanning = false;
    //    QStringList _deviceTypes;

    QList<QObject*> _deviceInfoList;

    void clearDevices();

    DeviceBaseClass::DeviceState m_deviceState;
};

#endif // DEVICEINTERFACE_H
