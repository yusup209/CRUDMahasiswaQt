#include "csvexporter.h"

CSVExporter::CSVExporter(QObject *parent)
    : QObject{parent}
    , m_delimiter(",")
    , m_bufferSize(0) // 0 means auto-size
    , m_autoBufferSize(true)
{}

/*************** public methods ***********************/

void CSVExporter::setDelimiter(const QString &delimiter)
{
    m_delimiter = delimiter;
}

void CSVExporter::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

void CSVExporter::setBufferSize(int size)
{
    m_bufferSize = size;
    m_autoBufferSize = (size == 0);
}

void CSVExporter::setAutoBufferSize(bool enable)
{
    m_autoBufferSize = enable;
    if (enable)
        m_bufferSize = 0;
}

bool CSVExporter::exportData(const QVector<QStringList> &data)
{
    if (m_filePath.isEmpty())
    {
        m_lastError = "File path is not set";
        return false;
    }

    if (data.isEmpty())
    {
        m_lastError = "No data to export";
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        m_lastError = "Cannot open file for writing: " + file.errorString();
        return false;
    }

    // Use buffered writing for better performance
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Calculate optimal buffer size if auto-sizing is enabled
    int bufferSize = m_autoBufferSize ? calculateOptimalBufferSize(data) : m_bufferSize;

    // Pre-allocate string buffer
    QString buffer;
    buffer.reserve(bufferSize);

    // Estimate average line length for better performance
    int estimatedLineLength = qMax(50, bufferSize / qMax(1, data.size()));
    int rowsPerFlush = qMax(1, bufferSize / estimatedLineLength);

    for (int i = 0; i < data.size(); ++i)
    {
        buildCsvLine(data[i], buffer);

        // Flush buffer periodically to avoid memory issues
        if (i > 0 && i % rowsPerFlush == 0 && !buffer.isEmpty())
        {
            out << buffer;
            buffer.clear();
            buffer.reserve(bufferSize);
        }
    }

    // Write remaining data
    if (!buffer.isEmpty())
    {
        out << buffer;
    }

    file.close();
    m_lastError.clear();
    return true;
}

bool CSVExporter::exportDataWithHeaders(const QStringList &headers, const QVector<QStringList> &data)
{
    QVector<QStringList> fullData;
    fullData.reserve(data.size() + 1);
    fullData.append(headers);
    fullData.append(data);
    return exportData(fullData);
}

QString CSVExporter::getLastError() const
{
    return m_lastError;
}

/*************** end of public methods ****************/


/*************** private methods **********************/

QString CSVExporter::escapeField(const QString &field) const
{
    // Quick check - if field doesn't need escaping, return as-is
    if (!field.contains(m_delimiter) &&
        !field.contains('"') &&
        !field.contains('\n') &&
        !field.contains('\r'))
    {
        return field;
    }

    // Field needs escaping
    QString escaped;
    escaped.reserve(field.length() + 10); // Pre-allocate with some extra space
    escaped.append('"');

    // Escape quotes by doubling them
    for (const QChar& ch : field)
    {
        if (ch == '"')
            escaped.append("\"\"");
        else
            escaped.append(ch);
    }

    escaped.append('"');
    return escaped;
}

void CSVExporter::buildCsvLine(const QStringList &row, QString &output) const
{
    if (row.isEmpty())
    {
        output.append('\n');
        return;
    }

    // First field
    output.append(escapeField(row[0]));

    // Remaining fields
    for (int i = 1; i < row.size(); ++i)
    {
        output.append(m_delimiter);
        output.append(escapeField(row[i]));
    }

    output.append('\n');
}

int CSVExporter::calculateOptimalBufferSize(const QVector<QStringList> &data) const
{
    if (data.isEmpty())
        return 8192; // 8KB minimum

    // Sample first few rows to estimate average row size
    int samplesToCheck = qMin(10, data.size());
    int totalEstimatedSize = 0;

    for (int i = 0; i < samplesToCheck; ++i)
    {
        int rowSize = 0;
        for (const QString& field : data[i])
        {
            rowSize += field.length() + 3; // +3 for delimiter, potential quotes, etc.
        }
        rowSize += 1; // newline
        totalEstimatedSize += rowSize;
    }

    int avgRowSize = totalEstimatedSize / samplesToCheck;
    int totalRows = data.size();

    // Define buffer size based on dataset size
    int bufferSize;

    if (totalRows < 100)
    {
        // Small dataset: 8KB - 16KB
        bufferSize = qMax(8192, avgRowSize * totalRows);
    }
    else if (totalRows < 1000)
    {
        // Medium dataset: 32KB - 64KB
        bufferSize = qMin(65536, qMax(32768, avgRowSize * 100));
    }
    else if (totalRows < 10000)
    {
        // Large dataset: 128KB
        bufferSize = 131072;
    }
    else if (totalRows < 100000)
    {
        // Very large dataset: 256KB
        bufferSize = 262144;
    }
    else
    {
        // Huge dataset: 512KB
        bufferSize = 524288;
    }

    // Cap at 1MB to avoid excessive memory usage
    return qMin(bufferSize, 1048576);
}

/*************** end of private methods ***************/
