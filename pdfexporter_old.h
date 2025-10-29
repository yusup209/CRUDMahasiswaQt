#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QString>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextFrameFormat>
#include <QTextLength>
#include <QElapsedTimer>
#include <QDateTime>
#include <QPrintDialog>
#include <QPrinter>

// Define the maximum number of rows to hold in memory before forcing a print and flushing the buffer.
constexpr int ROWS_PER_CHUNK = 500;
constexpr int NUM_COLUMNS = 4;

class PDFExporter : public QObject
{
    Q_OBJECT
public:
    explicit PDFExporter(QObject *parent = nullptr, const QString& title = "", const QString& label = "", const QList<QString>& columnHeaders = QList<QString>(), int bufferSize = ROWS_PER_CHUNK);

    bool openExport(const QString& filePath);
    void writeRow(const QList<QVariant>& rowData);
    void closeExport();

private:
    std::unique_ptr<QTextDocument> m_document; ///< The in-memory document buffer
    QPrinter m_printer;                        ///< The PDF file output handler

    const QString m_title;
    const QString m_label;
    const QList<QString> m_columnHeaders;
    int m_bufferSize;
    qint64 m_totalRowsWritten;
    bool m_isOpen;
    bool m_isFirstChunk;

    void flushBuffer();
    void initializeDocumentLayout(bool includeHeaderContent = true);


signals:
};

#endif // PDFEXPORTER_H
