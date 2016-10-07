#ifndef TELEGRAMCONFIG_H
#define TELEGRAMCONFIG_H

#define DC_CONFIG_SIGNED_DCID TelegramConfig::config()->signedDcId()
#define DC_CONFIG_UPDATES_STATE TelegramConfig::config()->updateState()
#define DC_CONFIG_SAVE TelegramConfig::config()->save()

#define TELEGRAM_CONFIG TelegramConfig::config()
#define IS_LOGGED_IN (DC_CONFIG_SIGNED_DCID != -1)

#define GET_DC_CONFIG_FROM_DCID(dcid) TelegramConfig::config()->dcConfig(dcid)
#define GET_DC_CONFIG_FROM_DC(dc) GET_DC_CONFIG_FROM_DCID(dc->id())
#define GET_DC_CONFIG_FROM_SESSION(dcsession) GET_DC_CONFIG_FROM_DC(dcsession->dc())

#include <QString>
#include <QHash>
#include "../autogenerated/types/types.h"
#include "../types/basic.h"
#include "dcconfig.h"

class TelegramConfig
{
    private:
        TelegramConfig();

    public:
        static TelegramConfig* config();
        static TelegramConfig* init(TLInt layernum, TLInt apiid, const QString& apihash, const QString& publickey, const QString &phonenumber);

    public:
        int signedDcId() const;
        DCConfig& dcConfig(int dcid);
        DCConfig& setDcConfig(int dcid, bool ipv6);
        UpdatesState* updateState();
        void save();
        void load();
        bool hasDC(int id);
        bool debugMode() const;
        TLInt layerNum() const;
        TLInt apiId() const;
        const QString& apiHash() const;
        const QString& publicKey() const;
        const QString& storagePath() const;
        const QString& deviceModel() const;
        const QString& osVersion() const;
        const QString& applicationVersion() const;
        const QString& phoneNumber() const;
        void setDebugMode(bool dbgmode);
        void setIpv6(bool ipv6);
        void setStoragePath(const QString& storagepath);
        void setDeviceModel(const QString& devicemodel);
        void setApplicationVersion(const QString& appversion);
        void setPhoneNumber(const QString& phonenumber);

    private:
        void saveDCConfig();
        void loadDCConfig();
        void updateStoragePath(const QString& storagepath, const QString& phonenumber);
        void write(const QString& filename, const QString& content);
        bool configExists(const QString& filename);
        QString read(const QString& filename);

    private:
        bool _debugmode;
        bool _ipv6;
        TLInt _layernum;
        TLInt _apiid;
        QString _apihash;
        QString _publickey;
        QString _storagepath;
        QString _profilestoragepath;
        QString _devicemodel;
        QString _osversion;
        QString _appversion;
        QString _phonenumber;
        QHash<int, DCConfig> _dcconfig;
        QHash<int, DCConfig> _dcconfigipv6;
        UpdatesState* _updatesstate;

    private:
        static const QString DCCONFIG_FILE;
        static TelegramConfig* _config;
};

#endif // TELEGRAMCONFIG_H
