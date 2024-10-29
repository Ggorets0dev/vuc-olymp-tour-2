#include <QCoreApplication>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QCommandLineParser>

#include "database.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    // Установка информации о приложении
    parser.setApplicationDescription("Приложение для импорта данных");
    parser.addHelpOption();  // Добавляет стандартный --help, который автоматически вызывает справку

    // Опции для импорта данных
    QCommandLineOption importEmployeesOption(
        "import-employees", "Импорт сотрудников из файла", "file");
    QCommandLineOption importStationsOption(
        "import-stations", "Импорт станций из файла", "file");
    QCommandLineOption importPositionsOption(
        "import-positions", "Импорт должностей из файла", "file");

    QCommandLineOption displayEmployeesOption(
        "display-employees", "Вывод всех сотрудников");
    QCommandLineOption displayStationsOption(
        "display-stations", "Вывод всех станций");
    QCommandLineOption displayPositionsOption(
        "display-positions", "Вывод всех должностей");

    QCommandLineOption checkStationFullnessOption(
        "check-fullness", "Проверка комплектования станций", "file");
    QCommandLineOption swapStationsOption(
        "swap-stations", "Смена комплектования станций");

    QCommandLineOption stationOneOption(
        "station-one", "Смена комплектования станций", "title");
    QCommandLineOption stationTwoOption(
        "station-two", "Смена комплектования станций", "title");

    QCommandLineOption titleSortOption(
        "title-sort", "Применить сортировку по названиям (при выводе)");

    QCommandLineOption taxDeductionOption(
        "tax-deduction", "Применить вычет налогов к зарплате (при выводе)", "percent");

    // Добавление опций в парсер
    parser.addOption(importEmployeesOption);
    parser.addOption(importStationsOption);
    parser.addOption(importPositionsOption);

    parser.addOption(displayEmployeesOption);
    parser.addOption(displayStationsOption);
    parser.addOption(displayPositionsOption);

    parser.addOption(checkStationFullnessOption);
    parser.addOption(swapStationsOption);

    parser.addOption(stationOneOption);
    parser.addOption(stationTwoOption);

    parser.addOption(titleSortOption);
    parser.addOption(taxDeductionOption);

    // Разбор аргументов командной строки
    parser.process(app);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    connectToDatabase(&db);

    // Проверка опции --import-employees
    if (parser.isSet(importEmployeesOption)) {
        QString filePath = parser.value(importEmployeesOption);
        importEmployees(filePath);
    }

    // Проверка опции --import-stations
    if (parser.isSet(importStationsOption)) {
        QString filePath = parser.value(importStationsOption);
        importStations(filePath);
    }

    // Проверка опции --import-positions
    if (parser.isSet(importPositionsOption)) {
        QString filePath = parser.value(importPositionsOption);
        importPositions(filePath);
    }

    if (parser.isSet(checkStationFullnessOption)) {
        QString filePath = parser.value(checkStationFullnessOption);
        if (checkStationsFullness(filePath)) {
            qInfo() << "Проверка на комплектование пройдена";
        } else {
            qInfo() << "Проверка на комплектование не пройдена";
        }
    }

    if (parser.isSet(swapStationsOption)) {
        if (!parser.isSet(stationOneOption) || parser.isSet(stationTwoOption)) {
            qInfo() << "Переданы не все аргументы, требующиеся для выполения операции";
        } else {
            if (swapStations(parser.value(stationOneOption), parser.value(stationTwoOption))) {
                qInfo() << "Персонал обеих станций успешно изменен";
            } else {
                qInfo() << "Не удалось изменить персонал обеих станций";
            }
        }
    }

    if (parser.isSet(displayEmployeesOption)) {
        displayEmployees(parser.isSet(titleSortOption));
    }

    if (parser.isSet(displayPositionsOption)) {
        displayPositions(parser.isSet(titleSortOption), parser.isSet(taxDeductionOption) ? parser.value(taxDeductionOption).toInt() : 0);
    }

    if (parser.isSet(displayStationsOption)) {
        displayStations(parser.isSet(titleSortOption));
    }

    db.close();
    qInfo() << "Отключение успешно!";

    return 0;
}
