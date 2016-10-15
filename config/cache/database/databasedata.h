#ifndef DATABASE_H
#define DATABASE_H

#include <functional>
#include <QtSql>

#define DatabaseConnection "__telegram_cache__"
#define GetCurrentDatabase QSqlDatabase::database(DatabaseConnection, false)
#define CreateQuery(name) QSqlQuery name(GetCurrentDatabase)

#endif // DATABASE_H
