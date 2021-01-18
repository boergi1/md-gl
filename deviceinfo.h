#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>

#include <QDebug>


class StreamProfile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString streamName READ streamName NOTIFY streamNameChanged)
    Q_PROPERTY(QStringList settingNames READ settingNames NOTIFY settingNamesChanged)
    Q_PROPERTY(QVariantList settingValues READ settingValues NOTIFY settingValuesChanged)

public:
    StreamProfile(QString streamName)
    {
        _streamName = streamName;
    }
    QString streamName() const
    {
        return _streamName;
    }
    QStringList settingNames() const
    {
        return _settingNames;
    }
    QVariantList settingValues() const
    {
        return _settingValues;
    }


    void addSetting(QString settingName, QVariantList settingValues)
    {
        _settingNames.append(settingName);
        _settingValues.append(QVariant::fromValue(settingValues));
        emit settingNamesChanged(_settingNames);
        emit settingValuesChanged(_settingValues);
    }

signals:
    void streamNameChanged(QString streamName);
    void settingNamesChanged(QStringList settingNames);
    void settingValuesChanged(QVariantList settingValues);

private:
    QString _streamName;
    QStringList _settingNames;
    QVariantList _settingValues;
};
class SensorOption;

class DeviceInfo : public QObject
{
    Q_OBJECT


    //    Q_PROPERTY(qint64 deviceFlags READ getDeviceFlags NOTIFY deviceFlagsChanged)

    //    Q_PROPERTY(bool connectionAlive READ connectionAlive NOTIFY aliveChanged)

    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int deviceState READ deviceState NOTIFY deviceStateChanged)
    Q_PROPERTY(QString deviceStateStr READ deviceStateStr NOTIFY deviceStateStrChanged)
    Q_PROPERTY(quint16 deviceType READ deviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(qint32 deviceIndex READ deviceIndex NOTIFY deviceIndexChanged)
    Q_PROPERTY(QString deviceIdentifier READ deviceIdentifier NOTIFY deviceIdentifierChanged)

    Q_PROPERTY(QVariant streamProfiles READ streamProfiles NOTIFY streamProfilesChanged)



public:
    DeviceInfo();

    QString deviceName() const;
    QString deviceStateStr() const;
    quint16 deviceType() const;
    qint32 deviceIndex() const;
    QString deviceIdentifier() const;

    void setDeviceName(QString name);
    void setDeviceState(int state)
    {
        _deviceState = state;
    }
    void setDeviceStateStr(QString state);
    void setDeviceType(quint16 type);
    void setDeviceIndex(qint32 index);
    void setDeviceIdentifier(QString identifier);

    QStringList streamSettings() const
    {
        return _streamProfileSettings;
    }

    QStringList sensorSettings() const
    {
        return _sensorSettings;
    }

    QVariant streamProfiles() const
    {
        return QVariant::fromValue(_streamProfiles);
    }

    bool isValidStreamProfile(QString streamName, QVariantList values)
    {
        return _streamProfilesData->contains(streamName, values);
    }

    int deviceState() const
    {
        return _deviceState;
    }

public slots:

    void updateStreamProfiles(QStringList settingNames, QMultiMap<QString, QVariantList>* streamProfiles)
    {
        _streamProfiles.clear();
        _streamProfilesData = streamProfiles;
        qDebug()<<"updateStreamProfiles, idx:"<<_deviceIndex<<"keys:"<<_streamProfilesData->uniqueKeys()<<"names:"<<settingNames;
        _streamProfileSettings = settingNames;

        auto streamNames = _streamProfilesData->uniqueKeys();
        for (auto name : streamNames)
        {
            auto profile = new StreamProfile(name);

            QMultiMap<QString, QVariant> unique_settings_map;
            auto map_i_spd = _streamProfilesData->find(name);
            while (map_i_spd != _streamProfilesData->end() && map_i_spd.key() == name)
            {
                auto values = &map_i_spd.value();
                for (int i=0; i<_streamProfileSettings.size(); i++)
                {
                    if ( !unique_settings_map.contains(_streamProfileSettings.at(i), values->at(i)) )
                        unique_settings_map.insert(_streamProfileSettings.at(i), values->at(i));
                }
                ++map_i_spd;
            }

            for (auto setting : _streamProfileSettings)
            {
                QVariantList settingValues;
                int it = 0;
                auto map_i_us = unique_settings_map.find(setting);
                while (map_i_us != unique_settings_map.end() && map_i_us.key() == setting)
                {
                    settingValues.append(map_i_us.value());
                    ++map_i_us;
                }
                profile->addSetting(setting, settingValues);

            }
            _streamProfiles.append(profile);
        }
        emit streamProfilesChanged();
    }

    void updateSensorOptions(QStringList settingNames, QMultiMap<QString, QVariantList>* sensorOptions)
    {
        _sensorOptionsData = sensorOptions;
        qDebug()<<"updateSensorOptions, idx:"<<_deviceIndex<<"keys:"<<_sensorOptionsData->uniqueKeys()<<"names:"<<settingNames;
        _sensorSettings = settingNames;
        // ("Name", "Description", "lBound", "uBound", "Step", "Default")

        //        // Print all sensor options
        //        qDebug()<<endl<<" - All Sensor Options -"<<endl;
        //        QMapIterator<QString, QList<QVariant>> i_map (*_sensorOptions);
        //        while (i_map.hasNext()) {
        //            i_map.next();
        //            qDebug() << i_map.key() << ": " << i_map.value();
        //        }
    }


signals:

    void deviceNameChanged(QString deviceName);
    void deviceStateStrChanged(QString deviceStateStr);
    void deviceTypeChanged(quint16 deviceType);
    void deviceIndexChanged(qint32 deviceIndex);
    void deviceIdentifierChanged(QString deviceIdentifier);

    void streamSettingsChanged(QStringList streamSettings);

    void sensorSettingsChanged(QStringList sensorSettings);

    void streamProfilesChanged();

    void sensorOptionsChanged();




    void deviceStateChanged(int deviceState);

private:


    QString _deviceName;
    QString _deviceStateStr;
    quint16 _deviceType;
    qint32 _deviceIndex;
    QString _deviceIdentifier;

    QList<StreamProfile*> _streamProfiles;

    QMultiMap<QString, QVariantList>* _streamProfilesData;
    QMultiMap<QString, QVariantList>* _sensorOptionsData;


    QMultiMap<QString, QVariant> uniqueStreamProfileSettings( QString streamName )
    {
        QMultiMap<QString, QVariant> temp_map;
        auto mmap_i = _streamProfilesData->find(streamName);
        while (mmap_i != _streamProfilesData->end() && mmap_i.key() == streamName)
        {
            auto values = &mmap_i.value();
            for (int i=0; i<_streamProfileSettings.size(); i++)
            {
                if ( !temp_map.contains(_streamProfileSettings.at(i), values->at(i)) )
                    temp_map.insert(_streamProfileSettings.at(i), values->at(i));
            }
            ++mmap_i;
        }
        return temp_map;
    }


    QStringList _streamProfileSettings;

    QStringList _sensorSettings;


    int _deviceState;
};





#endif // DEVICEINFO_H
