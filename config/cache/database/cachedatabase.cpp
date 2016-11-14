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

    // NOTE: Why I need those two pragmas?
    db.exec("PRAGMA synchronous = OFF");
    db.exec("PRAGMA journal_mode = MEMORY");

    this->loadTables();
}

ChatsTable *CacheDatabase::chats() const
{
    return this->_chatstable;
}

ChatUsersTable *CacheDatabase::chatUsers() const
{
    return this->_chatuserstable;
}

DialogsTable *CacheDatabase::dialogs() const
{
    return this->_dialogstable;
}

MessagesTable *CacheDatabase::messages() const
{
    return this->_messagestable;
}

PendingWebPageTable *CacheDatabase::pendingWebPages() const
{
    return this->_pendingwebpagestable;
}

UsersTable *CacheDatabase::users() const
{
    return this->_userstable;
}

void CacheDatabase::transaction(std::function<void (QSqlQuery &)> transactionproc)
{
    if(!GetCurrentDatabase.transaction())
        qWarning() << "Database doesn't support transactions";

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
    this->_chatuserstable = new ChatUsersTable(this);
    this->_dialogstable = new DialogsTable(this);
    this->_messagestable = new MessagesTable(this);
    this->_pendingwebpagestable = new PendingWebPageTable(this);
    this->_userstable = new UsersTable(this);

    this->_chatstable->createSchema();
    this->_chatuserstable->createSchema();
    this->_dialogstable->createSchema();
    this->_messagestable->createSchema();
    this->_pendingwebpagestable->createSchema();
    this->_userstable->createSchema();
}
