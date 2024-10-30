#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtDebug>
#include <QHash>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

using StationPersonnelCount = QHash<QString, qint8>;
using StationSwapCandidates = QPair<quint8, quint8>;

extern bool
connectToDatabase(QSqlDatabase* db);

extern bool
checkStationsFullness(const QString& filePath);

extern bool
swapStations(const qint8 Id1, const qint8 Id2);

extern bool
swapStations(const QString& title1, const QString& title2);

extern bool
checkStationCandidates(const StationSwapCandidates& conds);

// -------- DISPLAY CONTAINS OF DATABASE

extern void
displayEmployees(bool isTitleSort);

extern void
displayPositions(bool isTitleSort, int taxPercent = 0);

extern void
displayStations(bool isTitleSort);

// --------

// -------- IMPORT DATA FROM JSON

extern void
importStations(const QString& filePath);

extern void
importPositions(const QString& filePath);

extern void
importEmployees(const QString& filePath);

// --------

#endif // DATABASE_H
