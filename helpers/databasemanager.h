#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QList>
#include <QSqlRecord>
#include <QSqlError>
#include <helpers/Environments.h>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(const QString &databasePath, QObject *parent = nullptr);
    ~DatabaseManager();

    bool isDatabaseOpen() const;

    // --- Fungsi CRUD Universal ---

    // INSERT (Mengembalikan ID baris yang dimasukkan, atau -1 jika gagal)
    qint64 insertRecord(const QString &tableName, const QVariantMap &data);

    // SELECT (Mengembalikan daftar peta hasil)
    QList<StudentsDataStruct> selectRecords(const QString &tableName,
                                     const QStringList &columns, // <--- TERIMA PARAMETER INI
                                     const QString &condition = "",
                                     const QVariantMap &bindValues = QVariantMap());

    QVector<QStringList> selectRecordsToVector(const QString &tableName,
                                               const QStringList &columns, // <--- TERIMA PARAMETER INI
                                               const QString &condition = "",
                                               const QVariantMap &bindValues = QVariantMap());

    // UPDATE (Mengembalikan true jika berhasil)
    bool updateRecord(const QString &tableName,
                      const QVariantMap &data,
                      const QString &condition,
                      const QVariantMap &bindValues = QVariantMap());

    // DELETE (Mengembalikan true jika berhasil)
    bool deleteRecord(const QString &tableName,
                      const QString &condition,
                      const QVariantMap &bindValues = QVariantMap());

private:
    QSqlDatabase m_db;
    QString m_databasePath;

    bool openDatabase();
    void createTablesIfNotExist();
    void logError(const QString &function, const QSqlError &error);
};

#endif // DATABASEMANAGER_H
