#include "tablemodel.h"

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void TableModel::setColumns(QStringList cols)
{
    m_headers = cols;
}

void TableModel::setTableData(const QList<StudentsDataStruct> &data)
{
    // Notifikasi ke View bahwa data akan berubah
    beginResetModel();

    m_tableData = data;

    // Notifikasi ke View bahwa perubahan data sudah selesai
    endResetModel();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_tableData.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_headers.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    // if (!index.isValid() || index.row() >= m_tableData.size() || index.column() >= m_headers.size()) {
    //     return QVariant();
    // }

    // // Ambil baris (row)
    // const QVariantMap &rowData = m_tableData.at(index.row());

    // // Ambil nama kolom (header)
    // const QString &columnKey = m_headers.at(index.column());

    // if (role == Qt::DisplayRole || role == Qt::EditRole) {
    //     // Mengembalikan nilai berdasarkan nama kolom
    //     return rowData.value(columnKey);
    // }

    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole){
        if (col == 0) return QString(m_tableData.at(row).nama);
        if (col == 1) return QString(m_tableData.at(row).npm);
        if (col == 2) return QString(m_tableData.at(row).kelas);
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < m_headers.size()) {
            return m_headers.at(section);
        }
    } else if (orientation == Qt::Vertical) {
        // Anda bisa mengembalikan nomor baris atau membiarkannya kosong
        return section + 1;
    }

    return QVariant();
}

StudentsDataStruct TableModel::getCurrentData(int index)
{
    if (index < 0){
        StudentsDataStruct d;
        return d;
    }

    StudentsDataStruct d = m_tableData.at(index);
    return d;
}
