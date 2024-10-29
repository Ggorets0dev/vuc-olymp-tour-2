#include "database.h"

#define DB_NAME             "VoenTour2"
#define DB_USER_NAME        "user1"
#define DB_USER_PASSWORD    "user1"

#define DB_ERROR_MESSAGE    "Ошибка при работе с БД"

static QJsonDocument
readJsonFromFile(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл:" << filePath;
        return QJsonDocument();
    }

    // Чтение содержимого файла
    QByteArray fileData = file.readAll();
    file.close();

    // Преобразование содержимого файла в JSON документ
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);

    if (jsonDoc.isNull()) {
        qWarning() << "Ошибка при разборе JSON файла";
        return QJsonDocument();
    }

    return jsonDoc;
}

static QString
getPositionTitle(quint8 pos_id) {
    QSqlQuery query;

    query.prepare("SELECT title FROM positions WHERE id = :posid");

    query.bindValue(":posid", pos_id);

    if (!query.exec()) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return "";
    }

    query.next();

    return query.value(0).toString();
}

static QString
getStationTitle(quint8 st_id) {
    QSqlQuery query;

    query.prepare("SELECT title FROM stations WHERE id = :stid");

    query.bindValue(":stid", st_id);

    if (!query.exec()) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return "";
    }

    query.next();

    return query.value(0).toString();
}

static quint8
getStationId(const QString& title) {
    QSqlQuery query;

    query.prepare("SELECT id FROM stations WHERE title = :stt");

    query.bindValue(":stt", title);

    if (!query.exec()) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return -1;
    }

    query.next();

    return query.value(0).toUInt();
}

bool
connectToDatabase(QSqlDatabase* db) {
    // Настраиваем параметры подключения
    db->setHostName("localhost");           // Хост, где находится база данных
    db->setDatabaseName(DB_NAME);           // Имя базы данных
    db->setUserName(DB_USER_NAME);          // Имя пользователя
    db->setPassword(DB_USER_PASSWORD);      // Пароль

    // Открываем соединение
    if (!db->open()) {
        qInfo() << DB_ERROR_MESSAGE << ":" << db->lastError().text();
        return false;
    }

    qInfo() << "Подключение успешно!";

    return true;
}

void
displayEmployees(bool isTitleSort) {
    QSqlQuery query;

    QString queryText = "SELECT id, full_name, start_date, station_id, position_id FROM employees";

    if (isTitleSort) {
        queryText += " ORDER BY full_name ASC";
    }

    if (!query.exec(queryText)) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString fullName = query.value(1).toString();
        QString startDate = query.value(2).toString();

        qInfo() << "ID: " << id;
        qInfo() << "ФИО: " << fullName;
        qInfo() << "Должность: " << getPositionTitle(query.value(4).toUInt());
        qInfo() << "Подразделение: " << getStationTitle(query.value(3).toUInt());
        qInfo() << "Дата поступления на службу: " << startDate;

        qInfo() << "----------------------";
    }
}

void
displayStations(bool isTitleSort) {
    QSqlQuery query;

    QString queryText = "SELECT id, title, city, address FROM stations";

    if (isTitleSort) {
        queryText += " ORDER BY title ASC";
    }

    if (!query.exec(queryText)) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString title = query.value(1).toString();
        QString city = query.value(2).toString();
        QString address = query.value(3).toString();

        qInfo() << "ID: " << id;
        qInfo() << "Название: " << title;
        qInfo() << "Город: " << city;
        qInfo() << "Адрес: " << address;

        qInfo() << "----------------------";
    }
}

void
displayPositions(bool isTitleSort) {
    QSqlQuery query;

    QString queryText = "SELECT id, title, add_date, salary FROM positions";

    if (isTitleSort) {
        queryText += " ORDER BY title ASC";
    }

    if (!query.exec(queryText)) {
        qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString title = query.value(1).toString();
        QString addDate = query.value(2).toString();
        int salary = query.value(3).toInt();

        qInfo() << "ID: " << id;
        qInfo() << "Название: " << title;
        qInfo() << "Дата введения: " << addDate;
        qInfo() << "Заработная плата: " << salary << " руб.";

        qInfo() << "----------------------";
    }
}

void
importStations(const QString& filePath) {
    auto doc = readJsonFromFile(filePath);

    QSqlQuery query;

    // Подготовка запроса для вставки данных
    query.prepare("INSERT INTO stations (title, city, address) VALUES (:title, :city, :address)");

    // Проход по JSON массиву
    for (const QJsonValue &value : doc.array()) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject stationObj = value.toObject();

        QString title = stationObj.value("title").toString();
        QString city = stationObj.value("city").toString();
        QString address = stationObj.value("address").toString();

        // Привязка параметров для вставки
        query.bindValue(":title", title);
        query.bindValue(":city", city);
        query.bindValue(":address", address);

        // Выполнение запроса
        if (!query.exec()) {
            qWarning() << "Ошибка вставки данных: " << query.lastError().text();
        }
    }

    qInfo() << "Импорт успешно завершен!";
}

void
importPositions(const QString& filePath) {
    auto doc = readJsonFromFile(filePath);

    QSqlQuery query;

    // Подготовка запроса для вставки данных
    query.prepare("INSERT INTO positions (title, add_date, salary) VALUES (:title, :addDate, :salary)");

    // Проход по JSON массиву
    for (const QJsonValue &value : doc.array()) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject stationObj = value.toObject();

        QString title = stationObj.value("title").toString();
        QString addDate = stationObj.value("add_date").toString();
        quint32 salary = stationObj.value("salary").toInt();

        // Привязка параметров для вставки
        query.bindValue(":title", title);
        query.bindValue(":addDate", addDate);
        query.bindValue(":salary", salary);

        // Выполнение запроса
        if (!query.exec()) {
            qWarning() << "Ошибка вставки данных: " << query.lastError().text();
        }
    }

    qInfo() << "Импорт успешно завершен!";
}

void
importEmployees(const QString& filePath) {
    auto doc = readJsonFromFile(filePath);

    QSqlQuery query;

    // Подготовка запроса для вставки данных
    query.prepare("INSERT INTO employees (full_name, start_date, station_id, position_id) VALUES (:fullName, :startDate, :stid, :posid)");

    // Проход по JSON массиву
    for (const QJsonValue &value : doc.array()) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject empObj = value.toObject();

        QString fullName = empObj.value("full_name").toString();
        QString startDate = empObj.value("start_date").toString();
        quint8 stationId = empObj.value("station_id").toInt();
        quint8 positionId = empObj.value("position_id").toInt();


        // Привязка параметров для вставки
        query.bindValue(":fullName", fullName);
        query.bindValue(":startDate", startDate);
        query.bindValue(":stid", stationId);
        query.bindValue(":posid", positionId);

        // Выполнение запроса
        if (!query.exec()) {
            qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        }
    }

    qInfo() << "Импорт успешно завершен!";
}

bool
checkStationsFullness(const QString& filePath) {
    StationPersonnelCount personCount;
    auto doc = readJsonFromFile(filePath);

    QSqlQuery query;

    query.prepare("SELECT COUNT(*), station_id FROM employees JOIN positions ON employees.position_id = positions.id WHERE positions.title = :positionTitle GROUP BY station_id");

    for (const QJsonValue &value : doc.array()) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject obj = value.toObject();

        QString position = obj.value("position").toString();
        quint8 count = obj.value("count").toInt();

        personCount[position] = count;
    }

    foreach (const QString &position, personCount.keys()) {
        quint8 needCount = personCount.value(position);

        query.bindValue(":positionTitle", position);

        // Выполнение запроса
        if (!query.exec()) {
            qInfo() << DB_ERROR_MESSAGE << ":" << query.lastError().text();
        }

        while (query.next()) {
            quint8 nowCount = query.value(0).toUInt();

            if (nowCount != needCount)
                return false;
        }
    }

    return true;
}

extern bool
swapStations(const QString& title1, const QString& title2) {
    return swapStations(getStationId(title1), getStationId(title2));
}

extern bool
swapStations(const qint8 Id1, const qint8 Id2) {
    QSqlQuery query;

    // Начинаем транзакцию для обеспечения целостности данных
    if (!query.exec("BEGIN TRANSACTION")) {
        qWarning() << "Не удалось начать транзакцию:" << query.lastError();
        return false;
    }

    // Шаг 1: Устанавливаем `station_id = tempStationId` для всех записей с `station_id = stationId1`
    query.prepare("UPDATE employees SET station_id = NULL WHERE station_id = :stationId1");
    query.bindValue(":stationId1", Id1);
    if (!query.exec()) {
        qWarning() << "Ошибка при обновлении station_id =" << Id1 << ":" << query.lastError();
        query.exec("ROLLBACK");  // Откатываем изменения, если произошла ошибка
        return false;
    }

    // Шаг 2: Устанавливаем `station_id = stationId1` для всех записей с `station_id = stationId2`
    query.prepare("UPDATE employees SET station_id = :stationId1 WHERE station_id = :stationId2");
    query.bindValue(":stationId1", Id1);
    query.bindValue(":stationId2", Id2);
    if (!query.exec()) {
        qWarning() << "Ошибка при обновлении station_id =" << Id2 << ":" << query.lastError();
        query.exec("ROLLBACK");  // Откатываем изменения, если произошла ошибка
        return false;
    }

    // Шаг 3: Устанавливаем `station_id = stationId2` для всех записей с `station_id = tempStationId`
    query.prepare("UPDATE employees SET station_id = :stationId2 WHERE station_id IS NULL");
    query.bindValue(":stationId2", Id2);
    if (!query.exec()) {
        qWarning() << "Ошибка при обновлении временного station_id:" << query.lastError();
        query.exec("ROLLBACK");  // Откатываем изменения, если произошла ошибка
        return false;
    }

    // Завершаем транзакцию
    if (!query.exec("COMMIT")) {
        qWarning() << "Ошибка при завершении транзакции:" << query.lastError();
        return false;
    }

    return true;
}
