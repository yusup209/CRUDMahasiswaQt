#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QTextStream>

class CSVExporter : public QObject
{
    Q_OBJECT
public:
    explicit CSVExporter(QObject *parent = nullptr);
    // Set the delimiter (default is comma)
    void setDelimiter(const QString& delimiter);

    // Set the output file path
    void setFilePath(const QString& filePath);

    // Set buffer size in bytes (default: auto-adjust based on data)
    // Set to 0 to enable auto-sizing (default)
    void setBufferSize(int size);

    // Enable/disable automatic buffer sizing (enabled by default)
    void setAutoBufferSize(bool enable);

    // Export data to CSV file
    // data: 2D vector where each inner vector represents a row
    // Returns true if successful, false otherwise
    bool exportData(const QVector<QStringList>& data);

    // Convenience method to export with headers
    bool exportDataWithHeaders(const QStringList& headers,
                               const QVector<QStringList>& data);

    // Get the last error message
    QString getLastError() const;

private:
    QString m_delimiter;
    QString m_filePath;
    QString m_lastError;
    int m_bufferSize;
    bool m_autoBufferSize;

    // Escape field if it contains delimiter, quotes, or newlines
    QString escapeField(const QString& field) const;

    // Build CSV line from row
    void buildCsvLine(const QStringList& row, QString& output) const;

    // Calculate optimal buffer size based on data
    int calculateOptimalBufferSize(const QVector<QStringList>& data) const;
};

#endif // CSVEXPORTER_H
