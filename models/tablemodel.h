#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QList>
#include <helpers/Environments.h>
#include <QVariantMap>
#include  <QObject>

// Model ini mengasumsikan semua baris memiliki set kunci (kolom) yang sama.
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = nullptr);

    void setColumns(QStringList cols);

    // Fungsi untuk mengisi data model dari luar (e.g., dari DatabaseManager)
    void setTableData(const QList<StudentsDataStruct> &data);

    // Implementasi QAbstractTableModel wajib
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    StudentsDataStruct getCurrentData(int index);

private:
    QList<StudentsDataStruct> m_tableData; // Data baris (list of QVariantMap)
    QStringList m_headers;          // Nama kolom (headers)
};

#endif // TABLEMODEL_H
