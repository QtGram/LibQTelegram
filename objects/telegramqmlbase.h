#ifndef TELEGRAMQMLBASE_H
#define TELEGRAMQMLBASE_H

#define TG_VERSION_MAJOR 1
#define TG_VERSION_MINOR 0

#define TG_IMPORT(uri) uri.toUtf8().constData()

#define REGISTER_TYPE(type) \
    qRegisterMetaType<type>(#type)

#define REGISTER_OBJECT(objectname) \
    qmlRegisterType<objectname>(TG_IMPORT(uri), TG_VERSION_MAJOR, TG_VERSION_MINOR, #objectname)

#define REGISTER_OBJECT_AS(objectname, name) \
    qmlRegisterType<objectname>(TG_IMPORT(uri), TG_VERSION_MAJOR, TG_VERSION_MINOR, name)

#include <QtQml>
#include <QObject>
#include <QString>

class TelegramQmlBase
{
    private:
        TelegramQmlBase();

    public:
        static void initialize(const QString& uri);
};

#endif // TELEGRAMQMLBASE_H
