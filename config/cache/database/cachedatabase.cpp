#include "cachedatabase.h"
#include "databasedata.h"
#include <QStandardPaths>
#include <QDir>

#define DatabaseFile "cache.db"

CacheDatabase::CacheDatabase(const QString &configpath, QObject *parent) : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", DatabaseConnection);
    db.setDatabaseName(QDir(configpath).absoluteFilePath(DatabaseFile));
    db.open();

    this->loadTables();
}

ChatsTable *CacheDatabase::chats() const
{
    return this->_chatstable;
}

DialogsTable *CacheDatabase::dialogs() const
{
    return this->_dialogstable;
}

MessagesTable *CacheDatabase::messages() const
{
    return this->_messagestable;
}

UsersTable *CacheDatabase::users() const
{
    return this->_userstable;
}

void CacheDatabase::transaction(std::function<void (QSqlQuery &)> transactionproc)
{
    GetCurrentDatabase.transaction();
    CreateQuery(queryobj);

    transactionproc(queryobj);
    GetCurrentDatabase.commit();
}

CacheDatabase::~CacheDatabase()
{
    QSqlDatabase db = GetCurrentDatabase;

    if(db.isOpen())
        db.close();

    db = QSqlDatabase(); // Reset Database Reference
    QSqlDatabase::removeDatabase(DatabaseConnection);
}

void CacheDatabase::loadTables()
{
    this->_chatstable = new ChatsTable(this);
    this->_dialogstable = new DialogsTable(this);
    this->_messagestable = new MessagesTable(this);
    this->_userstable = new UsersTable(this);

    this->_chatstable->createSchema();
    this->_dialogstable->createSchema();
    this->_messagestable->createSchema();
    this->_userstable->createSchema();
}
