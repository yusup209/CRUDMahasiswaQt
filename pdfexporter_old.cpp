#include "pdfexporter.h"

PDFExporter::PDFExporter(QObject *parent, const QString& title, const QString& label, const QList<QString>& columnHeaders, int bufferSize)
    : QObject{parent},
      m_title(title), m_label(label), m_columnHeaders(columnHeaders), m_bufferSize(bufferSize), m_totalRowsWritten(0), m_isOpen(false), m_isFirstChunk(true)
{
    if (m_bufferSize <= 0) m_bufferSize = ROWS_PER_CHUNK;
}

bool PDFExporter::openExport(const QString &filePath)
{
    m_printer.setOutputFileName(filePath);
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    m_printer.setPageSize(QPageSize(QPageSize::A4));
    m_printer.setPageOrientation(QPageLayout::Portrait);
    m_printer.setFullPage(false);

    // Reset state
    m_document.reset(new QTextDocument());
    m_totalRowsWritten = 0;
    m_isFirstChunk = true;
    m_isOpen = true;

    qDebug() << "PDF Export initialized for:" << filePath;
    return true;
}

void PDFExporter::writeRow(const QList<QVariant> &rowData)
{
    if (!m_isOpen || !m_document) {
        qWarning() << "Cannot write row: Export stream is not open.";
        return;
    }

    if (m_isFirstChunk) {
        initializeDocumentLayout();
        m_isFirstChunk = false;
    }

    // --- FIX: Find the table robustly by traversing the document's root frame. ---
    // This ensures we get the table pointer even after a new document chunk is initialized.
    QTextTable* table = nullptr;
    QTextFrame *rootFrame = m_document->rootFrame();

    // Iterate through the root frame's elements to find the first table
    QTextFrame::iterator it;
    for (it = rootFrame->begin(); !it.atEnd(); ++it) {
        // CORRECTED: Use dynamic_cast on the frame element, as QTextTable inherits QTextFrame.
        // QTextFrame::iterator does not have a currentTable() method.
        QTextTable* potentialTable = dynamic_cast<QTextTable*>(it.currentFrame());
        if (potentialTable) {
            table = potentialTable;
            break;
        }
    }
    // --- END FIX ---

    if (table) {
        table->appendRows(1);
        int newRowIndex = table->rows() - 1;

        for (int col = 0; col < qMin(rowData.size(), NUM_COLUMNS); ++col) {
            // Get a cursor inside the new cell and insert text
            QTextCursor cellCursor = table->cellAt(newRowIndex, col).firstCursorPosition();
            cellCursor.insertText(rowData.at(col).toString());
        }
        m_totalRowsWritten++;
    } else {
        qCritical() << "Table not found in document. Cannot insert data. (Initialization failure?)";
        m_isOpen = false;
        return;
    }


    // Check for buffer overflow and flush automatically
    if ((m_totalRowsWritten % m_bufferSize == 0) && (m_totalRowsWritten > 0)) {
        flushBuffer();
        // After flushing, re-initialize the document buffer with the table and header
        initializeDocumentLayout(false); // Do not print main title/label again
    }
}

void PDFExporter::closeExport()
{
    if (!m_isOpen) {
        qWarning() << "Export stream is already closed.";
        return;
    }

    // Flush any remaining data in the buffer
    if (m_totalRowsWritten % m_bufferSize != 0) {
        flushBuffer();
    }


    qDebug() << Q_FUNC_INFO << "PDF Export finished. ";

    // The printer is automatically closed/finalized when it goes out of scope,
    // but we can rely on Qt's mechanism by simply setting m_isOpen to false.
    m_isOpen = false;
    m_document.reset(); // Explicitly free document memory
}

/********** private method  **************/
void PDFExporter::flushBuffer()
{
    if (!m_document) return;

    QElapsedTimer timer;
    timer.start();

    qDebug() << "Flushing chunk to PDF. Rows in chunk:" << (m_totalRowsWritten % m_bufferSize == 0 ? m_bufferSize : m_totalRowsWritten % m_bufferSize);

    // Print the document content to the current page/section of the PDF file
    m_document->print(&m_printer);

    // CRITICAL: Reset the document to free memory
    m_document.reset(new QTextDocument());

    qDebug() << "Chunk flushed in" << timer.elapsed() << "ms.";
}

void PDFExporter::initializeDocumentLayout(bool includeHeaderContent)
{
    QTextCursor cursor(m_document.get());
    cursor.beginEditBlock();

    // 1. --- TITLE & LABELS (Only on the very first chunk) ---
    if (includeHeaderContent) {
        // Title Format
        QTextBlockFormat titleFormat;
        titleFormat.setAlignment(Qt::AlignCenter);
        QTextCharFormat titleCharFormat;
        titleCharFormat.setFontPointSize(24);
        titleCharFormat.setFontWeight(QFont::Bold);

        cursor.setBlockFormat(titleFormat);
        cursor.setCharFormat(titleCharFormat);
        cursor.insertText(m_title + "\n");

        // Label Format
        QTextBlockFormat labelFormat;
        labelFormat.setBottomMargin(20);
        QTextCharFormat labelCharFormat;
        labelCharFormat.setFontPointSize(12);

        cursor.setBlockFormat(labelFormat);
        cursor.setCharFormat(labelCharFormat);
        cursor.insertText(m_label + " (Generated: " + QDateTime::currentDateTime().toString() + ")\n\n");
    }

    // 2. --- DATA TABLE SETUP (Required for every chunk) ---
    QTextTableFormat tableFormat;
    tableFormat.setBorder(0.5);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(5);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    // Column width distribution
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 10) // ID
                << QTextLength(QTextLength::PercentageLength, 35) // Name
                << QTextLength(QTextLength::PercentageLength, 25) // Value
                << QTextLength(QTextLength::PercentageLength, 30); // Timestamp
    tableFormat.setColumnWidthConstraints(constraints);

    QTextTable* table = cursor.insertTable(1, NUM_COLUMNS, tableFormat);

    // 3. --- TABLE HEADER ROW ---
    QTextCharFormat headerCharFormat;
    headerCharFormat.setFontWeight(QFont::Bold);
    headerCharFormat.setBackground(QColor("#E0E0E0"));

    for (int col = 0; col < qMin(m_columnHeaders.size(), NUM_COLUMNS); ++col) {
        QTextCursor cellCursor = table->cellAt(0, col).firstCursorPosition();
        cellCursor.setCharFormat(headerCharFormat);
        cellCursor.insertText(m_columnHeaders.at(col));
    }

    cursor.endEditBlock();
}

/********** end of private method  **************/
