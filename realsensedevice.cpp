#include "realsensedevice.h"


RealsenseDevice::RealsenseDevice(int id, DeviceInterface *parent, rs2::device device) : DeviceBaseClass(id, parent)
{

    _device = device;
}

quint16 RealsenseDevice::deviceSyncType() { return _deviceSyncType; }

void RealsenseDevice::setDeviceSyncType(quint16 deviceSyncType) { _deviceSyncType = deviceSyncType; }

QList<quint32> RealsenseDevice::fpsList() { return _fpsList; }


QString RealsenseDevice::syncTypeToStr(quint16 syncType)
{
    switch (syncType)
    {
    case SyncTypeRS::DEFAULT: return "Default";
    case SyncTypeRS::MASTER: return "Master";
    case SyncTypeRS::SLAVE: return "Slave";
    default: return "Undefined";
    }
}

void RealsenseDevice::updateSettings()
{
    //    this->setTotalStreamProfiles( new QMultiMap<QString, QList<QVariant>>() );
    //    this->setTotalSensorOptions( new QMap<QString, QList<QVariant>>() );
    //    this->setStreamProfileSettings( QStringList()<<"Width"<<"Height"<<"FPS"<<"Format" );
    //    this->setSensorOptionsSettings( QStringList()<<"Name"<<"Description"<<"lBound"<<"uBound"<<"Step"<<"Default" );

    auto* streamProfilesRS = new QMultiMap<QString, QList<QVariant>>();
    auto* sensorOptions = new QMultiMap<QString, QList<QVariant>>();


    this->setStreamProfileSettings(QStringList()<<"Width"<<"Height"<<"FPS"<<"Format");
    this->setSensorOptionsSettings(QStringList()<<"Name"<<"Description"<<"Current"<<"lBound"<<"uBound"<<"Step"<<"Default");
    //    auto streamProfileSettings = QStringList()<<"Width"<<"Height"<<"FPS"<<"Format";
    //    auto sensorOptionsSettings = QStringList()<<"Name"<<"Description"<<"Current"<<"lBound"<<"uBound"<<"Step"<<"Default";

    // Stream information - all possible setting combinations (stream and sensor)
    auto advanced_device = _device.as<rs400::advanced_mode>();
    for (auto &sensor : advanced_device.query_sensors())
    {
        QString sensorName = sensor.get_info(RS2_CAMERA_INFO_NAME);
        // Sensor stream profiles
        for (auto &streamProfile : sensor.get_stream_profiles())
        {
            QString streamName =  QString::fromStdString(streamProfile.stream_name());
            int fps = streamProfile.fps();
            streamProfile.unique_id();
            streamProfile.stream_index();

            rs2::video_stream_profile videoStreamProfile = streamProfile.as<rs2::video_stream_profile>();
            int width = videoStreamProfile.width();
            int height = videoStreamProfile.height();
            rs2_format format = videoStreamProfile.format();

            QList<QVariant> values;// = QList<QVariant>() << width << height << fps << static_cast<int>(format);
            values.append(width);
            values.append(height);
            values.append(fps);
            values.append(static_cast<int>(format));

            streamProfilesRS->insert(streamName, values);
        }


        // Sensor options
        for (int i = 0; i < static_cast<int>(RS2_OPTION_COUNT); i++)
        {
            rs2_option option_type = static_cast<rs2_option>(i);
            if (sensor.supports(option_type))
            {
                const char* optionName = sensor.get_option_name(option_type);
                const char* optionDescription = sensor.get_option_description(option_type);
                float current_value = sensor.get_option(option_type);
                rs2::option_range range = sensor.get_option_range(option_type);
                if ( sensor.is_option_read_only(option_type))
                    qWarning()<<"Option is read only:" << optionName;
                QList<QVariant> values;
                values.append(optionName);
                values.append(optionDescription);;
                values.append(current_value);
                values.append(range.min);
                values.append(range.max);
                values.append(range.step);
                values.append(range.def);

                sensorOptions->insert(sensorName, values);
            }
        }

    }


    emit streamProfilesDataChanged(this->streamProfileSettings(), streamProfilesRS);
    emit sensorOptionsDataChanged(this->sensorOptionsSettings(), sensorOptions);




    //setStreamProfileNames(_totalStreamProfilesPtr->uniqueKeys());
    // streamProfileNames() = _totalStreamProfilesPtr->uniqueKeys();


    //    // move this data to deviceinterface and qml (uniqueProfilesForStream())
    //    qDebug()<<endl<<_streamProfileNames.count()<<"unique streams found:"<<_streamProfileNames;

    //    for (int i=0; i<_streamProfileNames.count(); i++)
    //    {
    //        qDebug()<<endl<<"Profile"<<_streamProfileNames.at(i);
    //        auto test = uniqueProfilesForStream(_streamProfileNames.at(i));
    //        qDebug()<<endl<<_streamProfileNames.at(i)<<" - Unique Stream Profiles -"<<endl;
    //        QMapIterator<QString, QVariant> mmap_i (test);
    //        while (mmap_i.hasNext()) {
    //            mmap_i.next();
    //            qDebug()<<"key:"<<mmap_i.key()<<"val:"<<mmap_i.value();
    //        }
    //    }

    //    // Print Device information
    //    qDebug()<<endl<<" - Device information -"<<endl;
    //    for (int i = 0; i < static_cast<int>(RS2_CAMERA_INFO_COUNT); i++)
    //    {
    //        rs2_camera_info info_type = static_cast<rs2_camera_info>(i);
    //        QDebug dbg(QtDebugMsg);
    //        dbg << rs2_camera_info_to_string(info_type) << ": ";

    //        if (_device.supports(info_type))
    //            dbg << advanced_device.get_info(info_type);
    //        else
    //            dbg << "N/A";
    //    }
    //    // Print all stream profiles
    //    qDebug()<<endl<<" - All Stream Profiles -"<<endl;
    //    QMapIterator<QString, QList<QVariant>> i_map (*_totalStreamProfilesPtr);
    //    while (i_map.hasNext()) {
    //        i_map.next();
    //        qDebug() << i_map.key() << ": " << i_map.value();
    //    }
    //    qDebug();



    if (this->mainState() == DeviceState::FOUND)
        this->setMainState(DeviceState::UNINIT);
}

void RealsenseDevice::writeDeviceStreamProfile(QString streamName, QVariantList values)
{
    qDebug()<<"setDeviceStreamProfile"<<streamName;

    auto advanced_device = _device.as<rs400::advanced_mode>();

    for (auto &sensor : advanced_device.query_sensors())
    {
        QString sensorName = sensor.get_info(RS2_CAMERA_INFO_NAME);
        qDebug()<<"Sensor"<<sensorName;

        for (auto &streamProfile : sensor.get_stream_profiles())
        {
            QString name =  QString::fromStdString(streamProfile.stream_name());
            qDebug()<<"Stream"<<name;
            if (streamName == name)
                qDebug()<<"FOUND";
        }
    }


}

void RealsenseDevice::initialize()
{
    qDebug()<<"init device"<<this->deviceIndex();
    // todo: fill with qml parameters, print rs2 camera output to extra window
    // never forget 3 sec delay between rs2pipe activation ..

    if (this->mainState() > DeviceBaseClass::DeviceState::UNINIT)
    {
        qWarning()<<"Device already initialized and/or streaming, deinitialize first";
        return;
    }



    auto advanced_dev = _device.as<rs400::advanced_mode>();
    auto advanced_sensors = advanced_dev.query_sensors();

    //    bool depth_found = false;
    //    bool color_found = false;
    rs2::sensor depth_sensor;
    rs2::sensor color_sensor;
    //    for (auto&& sensor : advanced_sensors) {
    //        std::string module_name = sensor.get_info(RS2_CAMERA_INFO_NAME);
    //        _found_sensors.append(QString::fromStdString(module_name));
    //        //        if (module_name == "Stereo Module") {
    //        //            depth_sensor = sensor;
    //        //            _depth_found = true;
    //        //        } else if (module_name == "RGB Camera") {
    //        //            color_sensor = sensor;
    //        //            _color_found = true;
    //        //        }
    //    }

    for (auto&& sensor : advanced_sensors) {
        std::string module_name = sensor.get_info(RS2_CAMERA_INFO_NAME);
        if (module_name == "Stereo Module") {
            depth_sensor = sensor;
            _depth_found = true;
        } else if (module_name == "RGB Camera") {
            color_sensor = sensor;
            _color_found = true;
        }
    }

    if (_depth_found)
    {
        depth_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0); // float _par_depth_autoexp = 0;
        depth_sensor.set_option(RS2_OPTION_EXPOSURE, 8500); // microseconds // float _par_depth_exptime = 8500;
        depth_sensor.set_option(RS2_OPTION_GAIN, 16); // float _par_depth_gain = 16;
        depth_sensor.set_option(RS2_OPTION_FRAMES_QUEUE_SIZE, 1); // float _par_depth_queuesize = 1;
        depth_sensor.set_option(RS2_OPTION_GLOBAL_TIME_ENABLED, 0); // bool _par_depth_globaltime = 0;
    }
    else if (_color_found)
    {
        color_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
        color_sensor.set_option(RS2_OPTION_EXPOSURE, 100); // microseconds
        color_sensor.set_option(RS2_OPTION_GAIN, 64);
        color_sensor.set_option(RS2_OPTION_FRAMES_QUEUE_SIZE, 1);
        color_sensor.set_option(RS2_OPTION_GLOBAL_TIME_ENABLED, 0);
    }
    else {
        //qCritical() << "(RealsenseDevice) No depth sensor found for rs camera #" << this->deviceIdentifier();
        return;
    }

    depth_sensor.set_option(RS2_OPTION_INTER_CAM_SYNC_MODE, 0);
    //    qDebug() << "(Rs2Device) Camera" << this->deviceType() << "#" << this->deviceIdentifier()
    //             << "set to SyncMode" << syncTypeToStr(depth_sensor.get_option(RS2_OPTION_INTER_CAM_SYNC_MODE));


    //  return;



    //    rs2::pipeline rs2_pipe;
    rs2::config rs2_cfg;
    rs2_cfg.enable_device(_device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
    //  rs2_cfg.enable_stream(RS2_STREAM_DEPTH, FRAME_WIDTH_DEPTH, FRAME_HEIGHT_DEPTH, RS2_FORMAT_Z16, FRAME_RATE_DEPTH);



#if RS_COLOR_ENABLED
    rs2_cfg.enable_stream(RS2_STREAM_COLOR);
#else
    rs2_cfg.disable_stream(RS2_STREAM_COLOR);
#endif
    //    cfg.enable_stream(RS2_STREAM_INFRARED, 0);// RS2_STREAM_GPIO
    //    cfg.enable_stream(RS2_STREAM_INFRARED, 1);

    //    rs2::pipeline_profile rs2_profile;
    _rs2_profile = _rs2_pipe.start(rs2_cfg);












    this->setMainState(DeviceBaseClass::DeviceState::INIT);


}

void RealsenseDevice::startCapture()
{

}

void RealsenseDevice::uninitialize()
{
    this->setMainState(DeviceBaseClass::DeviceState::STOP);
    _rs2_pipe.stop();
    this->setMainState(DeviceBaseClass::DeviceState::UNINIT);
}

//void RealsenseDevice::initializeStream()
//{

//    // todo: fill with qml parameters, print rs2 camera output to extra window
//    // never forget 3 sec delay between rs2pipe activation ..
//    if (this->deviceState() != DeviceBaseClass::DeviceState::UNINIT)
//    {
//        qWarning()<<"Device already initialized / streaming, deinitialize first";
//        return;
//    }
//    auto advanced_dev = _device.as<rs400::advanced_mode>();
//    auto advanced_sensors = advanced_dev.query_sensors();
//    //    bool depth_found = false;
//    //    bool color_found = false;
//    rs2::sensor depth_sensor;
//    rs2::sensor color_sensor;
////    for (auto&& sensor : advanced_sensors) {
////        std::string module_name = sensor.get_info(RS2_CAMERA_INFO_NAME);
////        _found_sensors.append(QString::fromStdString(module_name));
////        //        if (module_name == "Stereo Module") {
////        //            depth_sensor = sensor;
////        //            _depth_found = true;
////        //        } else if (module_name == "RGB Camera") {
////        //            color_sensor = sensor;
////        //            _color_found = true;
////        //        }
////    }
//    if (_depth_found)
//    {
//        depth_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0); // float _par_depth_autoexp = 0;
//        depth_sensor.set_option(RS2_OPTION_EXPOSURE, 8500); // microseconds // float _par_depth_exptime = 8500;
//        depth_sensor.set_option(RS2_OPTION_GAIN, 16); // float _par_depth_gain = 16;
//        depth_sensor.set_option(RS2_OPTION_FRAMES_QUEUE_SIZE, 1); // float _par_depth_queuesize = 1;
//        depth_sensor.set_option(RS2_OPTION_GLOBAL_TIME_ENABLED, 0); // bool _par_depth_globaltime = 0;
//    }
//    else if (_color_found)
//    {
//        color_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
//        color_sensor.set_option(RS2_OPTION_EXPOSURE, 100); // microseconds
//        color_sensor.set_option(RS2_OPTION_GAIN, 64);
//        color_sensor.set_option(RS2_OPTION_FRAMES_QUEUE_SIZE, 1);
//        color_sensor.set_option(RS2_OPTION_GLOBAL_TIME_ENABLED, 0);
//    }
//    else {
//        qCritical() << "(RealsenseDevice) No depth sensor found for rs camera #" << this->deviceIdentifier();
//        return;
//    }
//    depth_sensor.set_option(RS2_OPTION_INTER_CAM_SYNC_MODE, _deviceSyncType);
//    qDebug() << "(Rs2Device) Camera " << this->deviceType() << " #" << this->deviceIdentifier()
//             << " set to SyncMode " << syncTypeToStr(depth_sensor.get_option(RS2_OPTION_INTER_CAM_SYNC_MODE));
//    return;
//    //    rs2::pipeline rs2_pipe;
//    rs2::config rs2_cfg;
//    rs2_cfg.enable_device(this->deviceIdentifier().toStdString());
//    //  rs2_cfg.enable_stream(RS2_STREAM_DEPTH, FRAME_WIDTH_DEPTH, FRAME_HEIGHT_DEPTH, RS2_FORMAT_Z16, FRAME_RATE_DEPTH);
//#if RS_COLOR_ENABLED
//    rs2_cfg.enable_stream(RS2_STREAM_COLOR);
//#else
//    rs2_cfg.disable_stream(RS2_STREAM_COLOR);
//#endif
//    //    cfg.enable_stream(RS2_STREAM_INFRARED, 0);// RS2_STREAM_GPIO
//    //    cfg.enable_stream(RS2_STREAM_INFRARED, 1);
//    //    rs2::pipeline_profile rs2_profile;
//    _rs2_profile = _rs2_pipe.start(rs2_cfg);
//    this->setDeviceState(DeviceBaseClass::DeviceState::INIT);
//}



void RealsenseDevice::startCapture(const quint32 &index) {


    // this should be the real thread function
    if (this->mainState() == DeviceBaseClass::DeviceState::UNINIT)
    { qWarning()<<"(RealsenseDevice) Can't start capture. Initialize device first."; return; }
    if (this->mainState() > DeviceBaseClass::DeviceState::INIT)
    { qCritical()<<"(RealsenseDevice) Capture start called while already running."; return; }

    auto serial = _device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
    qDebug()<< "(RealsenseDevice) Capture started: #" << serial << "threadID:" << this->deviceThread.currentThreadId();
    //   this->setDeviceState(DeviceBaseClass::DeviceState::STREAM);


    while (this->mainState() == DeviceBaseClass::DeviceState::INIT || this->mainState() == DeviceBaseClass::DeviceState::RECORD) {

        rs2::frameset frameset = _rs2_pipe.wait_for_frames();
        qDebug()<< "(RealsenseDevice) Frame from " << serial <<" #" << frameset.get_frame_number() << " total size: " << frameset.get_data_size();

        //        auto cap_start = std::chrono::high_resolution_clock::now();

        auto profile = frameset.get_profile();
        //        auto test =  profile.stream_name();
        //        std::string caption(test);




        qDebug() << "Stream name:" << QString::fromStdString(profile.stream_name()) << "Stream type:" << rs2_stream_to_string(profile.stream_type()) << frameset.get_profile().stream_type() <<"Stream format:"
                 << rs2_format_to_string(profile.format()) << profile.format() << "Stream index:" << profile.stream_index();

        switch (frameset.get_profile().stream_type()) {
        case RS2_STREAM_ANY: qDebug()<<"Any"; break;
        case RS2_STREAM_DEPTH: qDebug()<<"Depth"; break;
        case RS2_STREAM_COLOR: qDebug()<<"Color"; break;
        case RS2_STREAM_INFRARED: qDebug()<<"Infrared"; break;
        case RS2_STREAM_FISHEYE: qDebug()<<"fisheye"; break;
        case RS2_STREAM_GYRO: qDebug()<<"gyro"; break;
        case RS2_STREAM_ACCEL: qDebug()<<"accel"; break;
        case RS2_STREAM_GPIO: qDebug()<<"gpio"; break;
        case RS2_STREAM_POSE: qDebug()<<"pose"; break;
        case RS2_STREAM_CONFIDENCE: qDebug()<<"confidence"; break;
        case RS2_STREAM_COUNT: qDebug()<<"count"; break;
        }


        //                for (auto sensor : frameset.get_profile())
        //                {

        //                }

        //  "Stereo Module" "RGB Camera" "Infrared Module"
        //        for (auto &sensor : _found_sensors)
        //        {
        //            if (sensor = )
        //        }

        frameset.get_infrared_frame();


        if (_depth_found)
            rs2::depth_frame depth_frame = frameset.get_depth_frame();


        if (_color_found)
            rs2::depth_frame depth_frame = frameset.get_depth_frame();





        if (this->mainState() == DeviceBaseClass::DeviceState::RECORD)
        {

            QString result;
            emit frameReady(this->deviceIndex(), result);
        }




        //        std::cout << "(Rs2Device) " << getCamTypeStr() << " capture took " << cap_end << " ms" << std::endl;
        //        std::setprecision(2);

    }

    _rs2_pipe.stop();
    qDebug()<< "(RealsenseDevice) Capture ended: #" << serial << "threadID:" << this->deviceThread.currentThreadId();
    this->setMainState(DeviceBaseClass::DeviceState::INIT);




}





//    while (m_active) {
//        rs2::frameset frameset = rs2_pipe.wait_for_frames();
//        //  rs2::frameset fs = frame.as<rs2::frameset>()

//        auto cap_start = std::chrono::high_resolution_clock::now();

//        //        rs2::depth_frame depth_frame = frameset.get_depth_frame();
//        //#if RS_COLOR_ENABLED
//        //        rs2::frame color_frame = frameset.get_color_frame();
//        //#endif

//#if RS_DEPTH_ENABLED
//        rs2_metadata_type depth_ts = 0;
//        auto depth_frame = frameset.get_depth_frame();
//        if (depth_frame.supports_frame_metadata(RS2_FRAME_METADATA_TIME_OF_ARRIVAL))
//            depth_ts = depth_frame.get_frame_metadata(RS2_FRAME_METADATA_TIME_OF_ARRIVAL);
//        double drift = depth_ts - m_last_frame_time;
//        m_last_frame_time = depth_ts;
//#endif
//#if RS_COLOR_ENABLED
//        rs2_metadata_type color_ts = 0;
//        auto color_frame = frameset.get_color_frame();
//        if (color_frame.supports_frame_metadata(RS2_FRAME_METADATA_TIME_OF_ARRIVAL))
//            color_ts = color_frame.get_frame_metadata(RS2_FRAME_METADATA_TIME_OF_ARRIVAL);
//#endif
//        /* RS2_FRAME_METADATA_FRAME_TIMESTAMP
//         * RS2_FRAME_METADATA_SENSOR_TIMESTAMP
//         * RS2_FRAME_METADATA_TIME_OF_ARRIVAL
//         * RS2_FRAME_METADATA_BACKEND_TIMESTAMP */

//        if (m_recording)
//        {

//#if (FILTER_DEPTH_RS_ENABLED && RS_DEPTH_ENABLED)
//#if RS_FILTER_DECIMATION_ENABLED
//            depth_frame = m_dec_filter.process(depth_frame);
//#endif
//#if RS_FILTER_THRESHOLD_ENABLED
//            depth_frame = m_thr_filter.process(depth_frame);
//#endif
//#if RS_FILTER_HOLEFILL_ENABLED
//            depth_frame = m_hole_filter.process(depth_frame);
//#endif
//#if RS_FILTER_SPATIAL_ENABLED
//            depth_frame = m_spat_filter.process(depth_frame);
//#endif
//#if RS_FILTER_TEMPORAL_ENABLED
//            depth_frame = m_temp_filter.process(depth_frame);
//#endif
//#endif

//#if RS_DEPTH_ENABLED
//            depth_frame.keep();
//            m_depth_frame_queue->addFrame(depth_frame);
//#endif
//#if RS_COLOR_ENABLED
//            color_frame.keep();
//            m_color_frame_queue->addFrame(color_frame);
//#endif
//#if VERBOSE
//            auto cap_end = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-cap_start).count();
//            if (cap_end >= drift)
//                std::cerr << "(Rs2Device) Processing takes longer than capturing" << std::endl;
//#endif
//#if (VERBOSE > 1)
//#if RS_COLOR_ENABLED
//            std::cout << "(Rs2Device) Color frame from cam " << getCamTypeStr() << " (" << color_frame.get_frame_number() << "): "
//                      << color_frame.get_width() << "x" << color_frame.get_height() << " ts: " << color_ts << std::endl;
//#endif
//#if RS_DEPTH_ENABLED
//            std::cout << "(Rs2Device) Depth frame from cam " << getCamTypeStr() << " (" << depth_frame.get_frame_number() << "): "
//                      << depth_frame.get_width() << "x" << depth_frame.get_height() << " ts: " << depth_ts << std::endl;
//#endif
//            std::cout << "(Rs2Device) " << getCamTypeStr() << " capture took " << cap_end << " ms" << std::endl;
//            std::setprecision(2);
//#endif

//        }
//    }
//    rs2_pipe.stop();
//    std::cout << "(Rs2Device) Capture thread stopped: #" << serial << " threadID: " << std::this_thread::get_id() << std::endl;
//}
