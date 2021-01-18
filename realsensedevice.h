#ifndef REALSENSEDEVICE_H
#define REALSENSEDEVICE_H

//#include <QObject>
#include "devicebaseclass.h"

//#include "deviceinterface.h"

#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>
#include <librealsense2/rs_advanced_mode.hpp>
#include <librealsense2/hpp/rs_frame.hpp>

#include <QMap>

//class StreamProfile : public QList<QVariant>
//{
//public:
//    StreamProfile(int width, int height, int fps, int format)
//    {
//        this->append(width);
//        this->append(height);
//        this->append(fps);
//        this->append(format);
//    }
//    StreamProfile(QList<QVariant> *streamProfile)
//    {
//        for (auto value : *streamProfile)
//            this->append(value);
//        if (this->size() != 4) qCritical()<<"StreamProfile wrong data size";
//    }
//    int getWidth() { return this->at(0).toInt(); }
//    int getHeight() { return this->at(1).toInt(); }
//    int getFps() { return this->at(2).toInt(); }
//    int getFormat() { return this->at(3).toInt(); }
//    QString getFormatString() { return rs2_format_to_string(static_cast<rs2_format>(this->at(3).toInt())); }
//};

//class SensorOption;

class DeviceInterface;


class RealsenseDevice : public DeviceBaseClass
{
    Q_OBJECT
    //    Q_PROPERTY(quint32 deviceSyncType READ deviceSyncType WRITE setDeviceSyncType NOTIFY deviceSyncTypeChanged)

    //    Q_PROPERTY(QStringList streamNames READ streamNames NOTIFY streamProfilesChanged)
    //    //    Q_PROPERTY(QStringList streamProfilesNames READ streamProfilesNames)
    //    Q_PROPERTY(QStringList sensorNames READ sensorNames NOTIFY sensorOptionsChanged)



    //   Q_PROPERTY(QList fpsListRS READ fpsList NOTIFY fpsListChanged)
public:
    RealsenseDevice(int id, DeviceInterface* parent, rs2::device device);




    enum SyncTypeRS {
        DEFAULT = 0,
        MASTER,
        SLAVE
    };
    Q_ENUM(SyncTypeRS)

    quint16 deviceSyncType();



    void setDeviceSyncType(quint16 deviceSyncType);

    QList<quint32> fpsList();

    QList<QString> streamNames();
    QStringList streamProfilesNames();
    QList<QString> sensorNames();





    //    QString devTypeToStr(quint16 deviceType)
    //    {
    //        switch (deviceType)
    //        {
    //        case DeviceType::CENTRAL: return "Central";
    //        case DeviceType::FRONT: return "Front";
    //        case DeviceType::REAR: return "Rear";
    //        default: return "Undefined";
    //        }
    //    }
    QString syncTypeToStr(quint16 syncType);

    QString formatToString(int format)
    {
        switch (static_cast<rs2_format>(format)) {
        case RS2_FORMAT_ANY: return "Any";
        case RS2_FORMAT_Z16: return "Z16";
        case RS2_FORMAT_DISPARITY16: return "Disp16F";
        case RS2_FORMAT_XYZ32F: return "XYZ32F";
        case RS2_FORMAT_YUYV: return "YUYV";
        case RS2_FORMAT_RGB8: return "RGB8";
        case RS2_FORMAT_BGR8: return "BGR8";
        case RS2_FORMAT_RGBA8: return "RGBA8";
        case RS2_FORMAT_BGRA8: return "BGRA8";
        case RS2_FORMAT_Y8: return "Y8";
        case RS2_FORMAT_Y16: return "Y16";
        case RS2_FORMAT_RAW10: return "RAW10";
        case RS2_FORMAT_RAW16: return "RAW16";
        case RS2_FORMAT_RAW8: return "RAW8";
        case RS2_FORMAT_UYVY: return "UYVY";
        case RS2_FORMAT_MOTION_RAW: return "M_RAW";
        case RS2_FORMAT_MOTION_XYZ32F: return "M_XYZ32F";
        case RS2_FORMAT_GPIO_RAW: return "GPIOraw";
        case RS2_FORMAT_6DOF: return "6DOF";
        case RS2_FORMAT_DISPARITY32: return "Disp32F";
        case RS2_FORMAT_Y10BPACK: return "Y10BPACK";
        case RS2_FORMAT_DISTANCE: return "Dist32F";
        case RS2_FORMAT_MJPEG: return "MJPEG";
        case RS2_FORMAT_Y8I: return "Y8I";
        case RS2_FORMAT_Y12I: return "Y12I";
        case RS2_FORMAT_INZI: return "INZI";
        case RS2_FORMAT_INVI: return "INVI";
        case RS2_FORMAT_W10: return "W10";
        case RS2_FORMAT_Z16H: return "Z16H";

        default: return "Format";
        }
    }

    //    QStringList _streamProfileNames;
    //    QStringList _streamProfileSettings;// = QStringList()<<"Width"<<"Height"<<"FPS"<<"Format";
    //    QStringList _sensorOptionsSettings;

    //    QMultiMap<QString, QList<QVariant>> *_totalStreamProfilesPtr;
    //    QMultiMap<QString, QList<QVariant>>  _uniqueStreamProfiles; // "Depth", (1280, 720, 30 , 0)
    //    QMap<QString, QList<QVariant>> *_totalSensorOptionsPtr;


public slots:
    virtual void updateSettings() override;

    virtual void writeDeviceStreamProfile(QString streamName, QVariantList values) override;


    virtual void initialize() override;
    virtual void startCapture() override;
    virtual void uninitialize() override;



    //   void initializeStream();
    void startCapture(const quint32 &index);




    //    void Rs2Device::print_device_information(const rs2::device &dev)
    //    {
    //        std::cout << "Device information: " << std::endl;

    //    }







signals:
    void frameReady(quint32 idx, const QString &result);
    void fpsListChanged();
    void deviceSyncTypeChanged();
    //    void streamProfilesChanged();
    //    void sensorOptionsChanged();



private:

    rs2::device _device;
    rs2::pipeline _rs2_pipe;
    rs2::pipeline_profile _rs2_profile;
    rs2::decimation_filter _filter_decimation;// = rs2::decimation_filter(RS_FILTER_DEC_MAG);
    rs2::threshold_filter _filter_threshold;// = rs2::threshold_filter(RS_FILTER_THR_MIN, RS_FILTER_THR_MAX);
    rs2::hole_filling_filter _filter_holefill;// = rs2::hole_filling_filter(RS_FILTER_HOLE_MODE);
    rs2::spatial_filter _filter_spatial;// = rs2::spatial_filter(RS_FILTER_SPA_ALPHA, RS_FILTER_SPA_DELTA, RS_FILTER_SPA_MAG, RS_FILTER_SPA_HOLE);
    rs2::temporal_filter _filter_temporal;// = rs2::temporal_filter(RS_FILTER_TEMP_ALPHA, RS_FILTER_TEMP_DELTA, RS_FILTER_TEMP_PERS);

    quint16 _deviceSyncType;


    QList<quint32> _fpsList = { 6,15,30,60,90 };
    QList<QPoint> _resolutionList = { QPoint(1280,720), QPoint(848,480), QPoint(640,480), QPoint(640,360), QPoint(480,270), QPoint(424,240) };



    QVector<QString> _found_sensors; // "Stereo Module" "RGB Camera" "Infrared Module" (?)

    float _par_depth_autoexp = 0;
    float _par_depth_exptime = 8500;
    float _par_depth_gain = 16;
    float _par_depth_queuesize = 1;
    bool _par_depth_globaltime = 0;

    bool _depth_found = false;
    bool _color_found = false;





};

#endif // REALSENSEDEVICE_H
