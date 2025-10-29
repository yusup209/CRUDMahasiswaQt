#include "databasemanager.h"
#include <QDebug>
#include <QFile>

DatabaseManager::DatabaseManager(const QString &databasePath, QObject *parent)
    : QObject(parent), m_databasePath(databasePath)
{
    if (openDatabase()) {
        createTablesIfNotExist();
    }
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
        QSqlDatabase::removeDatabase("qt_sql_default_connection"); // Hapus koneksi default
    }
}

bool DatabaseManager::openDatabase()
{
    bool isNewDatabase = !QFile::exists(m_databasePath);

    // Tambahkan koneksi ke database SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_databasePath);

    if (!m_db.open()) {
        qCritical() << "Gagal membuka database:" << m_db.lastError().text();
        return false;
    }

    if (isNewDatabase) {
        qDebug() << "Basis data baru dibuat di:" << m_databasePath;
    } else {
        qDebug() << "Koneksi ke basis data berhasil dibuka.";
    }

    return true;
}

void DatabaseManager::createTablesIfNotExist()
{
    QSqlQuery query(m_db);

    // Contoh pembuatan tabel 'mahasiswa'
    QString createUsersTable = "CREATE TABLE IF NOT EXISTS mahasiswa ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "nama TEXT NOT NULL UNIQUE, "
                               "npm TEXT, "
                               "kelas TEXT"
                               ")";

    if (!query.exec(createUsersTable)) {
        logError("createTablesIfNotExist (users)", query.lastError());
    } else {
        qDebug() << "Tabel 'users' dipastikan ada.";
    }

    // Tambahkan lebih banyak tabel di sini jika diperlukan
}

bool DatabaseManager::isDatabaseOpen() const
{
    return m_db.isOpen();
}

// Implementasi fungsi CRUD:

qint64 DatabaseManager::insertRecord(const QString &tableName, const QVariantMap &data)
{
    if (!m_db.isOpen() || data.isEmpty()) return -1;

    QStringList columns = data.keys();
    QString columnNames = columns.join(", ");
    QString placeholders;
    for (int i = 0; i < columns.size(); ++i) {
        placeholders += (i == 0 ? "" : ", ") + QString(":%1").arg(columns[i]);
    }

    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(columnNames).arg(placeholders);

    QSqlQuery query(m_db);
    query.prepare(sql);

    // Binding values
    foreach (const QString &key, columns) {
        query.bindValue(QString(":%1").arg(key), data.value(key));
    }

    if (!query.exec()) {
        logError("insertRecord", query.lastError());
        return -1;
    }

    return query.lastInsertId().toLongLong();
}

QList<StudentsDataStruct> DatabaseManager::selectRecords(const QString &tableName,
                                                  const QStringList &columns, // <--- TERIMA PARAMETER INI
                                                  const QString &condition,
                                                  const QVariantMap &bindValues)
{
    QList<StudentsDataStruct> rowData;
    // QList<QVariantMap> results;
    if (!m_db.isOpen()) return rowData;

    // Tentukan kolom mana yang akan dipilih
    QString columnList = "*"; // Default: semua kolom
    if (!columns.isEmpty()) {
        columnList = columns.join(", "); // Gabungkan kolom yang dipilih
    }

    QSqlQuery query(m_db);

    // buat query sql hanya untuk select count, jadi hasilnya sebagai patokan QList reserve (alokasi sekali aja).
    QString sqlCount = QString("SELECT COUNT(id) FROM %1;").arg(tableName);

    query.prepare(sqlCount);

    if (!query.exec()){
        logError("(select count) error : ", query.lastError());
        return rowData;
    }

    int jmlRecord = 0;
    if (query.next()){
        jmlRecord = query.record().value(0).toInt();
        // qDebug() << Q_FUNC_INFO << "jumlah recordnya adalah : " << jmlRecord;
    }


    // Bangun query SQL: SELECT [kolom] FROM [tabel] WHERE [kondisi]
    QString sql = QString("SELECT %1 FROM %2").arg(columnList).arg(tableName);
    if (!condition.isEmpty()) {
        sql += " WHERE " + condition;
    }

    // qDebug() << Q_FUNC_INFO << sql;

    query.prepare(sql);

    // Binding values untuk mencegah SQL Injection
    foreach (const QString &key, bindValues.keys()) {
        query.bindValue(key, bindValues.value(key));
    }

    if (!query.exec()) {
        logError("selectRecords", query.lastError());
        return rowData;
    }

    if (jmlRecord > 0){
        // Ambil hasil
        while (query.next()) {
            // QVariantMap row;
            StudentsDataStruct studentData;
            QSqlRecord record = query.record();

            // Ambil data berdasarkan nama field/kolom
            for (int i = 0; i < record.count(); ++i) {
                // Gunakan fieldName() dari QSqlRecord, yang hanya berisi kolom yang dipilih.
                if (record.fieldName(i).contains("id")){
                    studentData.id = record.value(i).toInt();
                } else if (record.fieldName(i).contains("nama")){
                    studentData.nama = record.value(i).toString();
                } else if (record.fieldName(i).contains("npm")){
                    studentData.npm = record.value(i).toString();
                } else if (record.fieldName(i).contains("kelas")){
                    studentData.kelas = record.value(i).toString();
                }
            }
            rowData.push_back(studentData);
        }
    }

    return rowData;
}

QVector<QStringList> DatabaseManager::selectRecordsToVector(const QString &tableName, const QStringList &columns, const QString &condition, const QVariantMap &bindValues)
{
    QVector<QStringList> rowData;
    // QList<QVariantMap> results;
    if (!m_db.isOpen()) return rowData;

    // Tentukan kolom mana yang akan dipilih
    QString columnList = "*"; // Default: semua kolom
    if (!columns.isEmpty()) {
        columnList = columns.join(", "); // Gabungkan kolom yang dipilih
    }

    QSqlQuery query(m_db);

    // buat query sql hanya untuk select count, jadi hasilnya sebagai patokan QList reserve (alokasi sekali aja).
    QString sqlCount = QString("SELECT COUNT(id) FROM %1").arg(tableName);
    query.prepare(sqlCount);
    if (!query.exec()){
        logError("(select count) error : ", query.lastError());
        return rowData;
    }

    int jmlRecord = 0;
    if (query.next()){
        jmlRecord = query.record().value(0).toInt();
        rowData.reserve(jmlRecord);
        qDebug() << Q_FUNC_INFO << "jumlah recordnya adalah : " << jmlRecord;
    }

    // Bangun query SQL: SELECT [kolom] FROM [tabel] WHERE [kondisi]
    QString sql = QString("SELECT %1 FROM %2").arg(columnList).arg(tableName);
    if (!condition.isEmpty()) {
        sql += " WHERE " + condition;
    }

    query.prepare(sql);

    // Binding values untuk mencegah SQL Injection
    foreach (const QString &key, bindValues.keys()) {
        query.bindValue(key, bindValues.value(key));
    }

    if (!query.exec()) {
        logError("selectRecords", query.lastError());
        return rowData;
    }

    if (jmlRecord > 0){
        // Ambil hasil
        while (query.next()) {
            // QVariantMap row;
            StudentsDataStruct studentData;
            QSqlRecord record = query.record();
            QStringList lst;

            // Ambil data berdasarkan nama field/kolom
            for (int i = 0; i < record.count(); ++i) {
                // Gunakan fieldName() dari QSqlRecord, yang hanya berisi kolom yang dipilih.
                if (record.fieldName(i).contains("id")){
                    lst.push_back(QString::number(record.value(i).toInt()));
                } else if (record.fieldName(i).contains("nama")){
                    lst.push_back(record.value(i).toString());
                } else if (record.fieldName(i).contains("npm")){
                    lst.push_back(record.value(i).toString());
                } else if (record.fieldName(i).contains("kelas")){
                    lst.push_back(record.value(i).toString());
                }
            }
            rowData.push_back(lst);
        }
    }

    return rowData;
}

bool DatabaseManager::updateRecord(const QString &tableName,
                                   const QVariantMap &data,
                                   const QString &condition,
                                   const QVariantMap &bindValues)
{
    if (!m_db.isOpen() || data.isEmpty() || condition.isEmpty()) return false;

    QStringList setClauses;
    foreach (const QString &key, data.keys()) {
        setClauses << QString("%1 = :upd_%1").arg(key);
    }

    QString sql = QString("UPDATE %1 SET %2 WHERE %3").arg(tableName).arg(setClauses.join(", ")).arg(condition);
    qDebug() << Q_FUNC_INFO << sql;

    QSqlQuery query(m_db);
    query.prepare(sql);

    // Binding values untuk SET
    foreach (const QString &key, data.keys()) {
        query.bindValue(QString(":upd_%1").arg(key), data.value(key));
    }

    // Binding values untuk WHERE
    foreach (const QString &key, bindValues.keys()) {
        query.bindValue(key, bindValues.value(key));
    }

    if (!query.exec()) {
        qDebug() << Q_FUNC_INFO << query.lastQuery();
        logError("updateRecord", query.lastError());
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::deleteRecord(const QString &tableName,
                                   const QString &condition,
                                   const QVariantMap &bindValues)
{
    if (!m_db.isOpen() || condition.isEmpty()) return false;

    QString sql = QString("DELETE FROM %1 WHERE %2").arg(tableName).arg(condition);

    QSqlQuery query(m_db);
    query.prepare(sql);

    // Binding values untuk WHERE
    foreach (const QString &key, bindValues.keys()) {
        query.bindValue(key, bindValues.value(key));
    }

    if (!query.exec()) {
        logError("deleteRecord", query.lastError());
        return false;
    }

    return query.numRowsAffected() > 0;
}

void DatabaseManager::logError(const QString &function, const QSqlError &error)
{
    qWarning() << "DatabaseManager Error (" << function << "):"
               << "Type:" << error.type()
               << "Text:" << error.text()
               << "Driver Text:" << error.driverText()
               << "Database Text:" << error.databaseText();
}
