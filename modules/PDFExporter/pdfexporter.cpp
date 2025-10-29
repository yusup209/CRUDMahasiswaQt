#include "pdfexporter.h"

PDFExporter::PDFExporter(QObject *parent)
    : QObject{parent}
    , m_showDate(true)
    , m_dateFormat("dd MMMM yyyy, hh:mm:ss")
    , m_pageSize(QPageSize::A4)
    , m_orientation(QPageLayout::Portrait)
    , m_headerColor("#D3D3D3")
    , m_zebraEvenColor("#FFFFFF")
    , m_zebraOddColor("#F0F0F0")
{
}

PDFExporter::~PDFExporter()
{
}

void PDFExporter::setTitle(const QString &title)
{
    m_title = title;
}

void PDFExporter::setShowDate(bool show)
{
    m_showDate = show;
}

void PDFExporter::setDateFormat(const QString &format)
{
    m_dateFormat = format;
}

void PDFExporter::setPageSize(QPageSize::PageSizeId pageSize)
{
    m_pageSize = pageSize;
}

void PDFExporter::setPageOrientation(QPageLayout::Orientation orientation)
{
    m_orientation = orientation;
}

void PDFExporter::setTableHeaders(const QStringList &headers)
{
    m_headers = headers;
}

void PDFExporter::setTableData(const QList<QStringList> &data)
{
    m_data = data;
}

void PDFExporter::setHeaderColor(const QString &color)
{
    m_headerColor = color;
}

void PDFExporter::setZebraColors(const QString &evenColor, const QString &oddColor)
{
    m_zebraEvenColor = evenColor;
    m_zebraOddColor = oddColor;
}

void PDFExporter::setCustomHtml(const QString &html)
{
    m_customHtml = html;
}

void PDFExporter::setPrefixHtml(const QString &html)
{
    m_prefixHtml = html;
}

void PDFExporter::setSuffixHtml(const QString &html)
{
    m_suffixHtml = html;
}

QString PDFExporter::escapeHtml(const QString &text) const
{
    QString escaped = text;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&#39;");
    return escaped;
}

QString PDFExporter::generateHtml()
{
    QString html;

    // Start HTML with proper styling
    html += "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; }";
    html += "h1 { color: #333333; margin-bottom: 10px; }";
    html += "p { color: #333333; margin: 5px 0; }";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 15px; table-layout: fixed; }";
    html += "th, td { border: 1px solid #000000; padding: 8px; text-align: left; word-wrap: break-word; }";
    html += "th { font-weight: bold; }";
    html += "hr { border: 1px solid #CCCCCC; margin: 15px 0; }";
    html += "</style></head><body>";

    // Title
    if (!m_title.isEmpty()) {
        html += QString("<h1>%1</h1>").arg(escapeHtml(m_title));
    }

    // Date
    if (m_showDate) {
        QString currentDate = QDateTime::currentDateTime().toString(m_dateFormat);
        html += QString("<p>Tanggal Dibuat: %1</p>").arg(currentDate);
    }

    html += "<hr/>";

    // Prefix HTML
    if (!m_prefixHtml.isEmpty()) {
        html += m_prefixHtml;
    }

    // Main content: Custom HTML or Table
    if (!m_customHtml.isEmpty()) {
        html += m_customHtml;
    } else if (!m_headers.isEmpty() && !m_data.isEmpty()) {
        html += generateTableHtml();
    }

    // Suffix HTML
    if (!m_suffixHtml.isEmpty()) {
        html += m_suffixHtml;
    }

    html += "</body></html>";

    m_lastGeneratedHtml = html;
    return html;
}

QString PDFExporter::generateTableHtml()
{
    QString tableHtml;

    tableHtml += "<table width='100%'>";

    // Header row
    if (!m_headers.isEmpty()) {
        tableHtml += QString("<tr style='background-color: %1;'>").arg(m_headerColor);
        for (const QString &header : m_headers) {
            tableHtml += QString("<th>%1</th>").arg(escapeHtml(header.toUpper()));
        }
        tableHtml += "</tr>";
    }

    // Data rows with zebra striping
    int rowCount = 0;
    for (const QStringList &row : m_data) {
        QString rowColor = (rowCount % 2 == 0) ? m_zebraEvenColor : m_zebraOddColor;
        tableHtml += QString("<tr style='background-color: %1;'>").arg(rowColor);

        // Handle column count mismatch gracefully
        int colCount = qMin(row.size(), m_headers.size());
        for (int i = 0; i < colCount; ++i) {
            tableHtml += QString("<td>%1</td>").arg(escapeHtml(row[i]));
        }

        // Fill missing columns with empty cells
        for (int i = colCount; i < m_headers.size(); ++i) {
            tableHtml += "<td></td>";
        }

        tableHtml += "</tr>";
        rowCount++;
    }

    tableHtml += "</table>";

    return tableHtml;
}

bool PDFExporter::preview(QWidget *parent)
{
    // Generate HTML content
    QString htmlContent = generateHtml();

    // Create QTextDocument
    QTextDocument doc;
    doc.setHtml(htmlContent);

    // Setup printer for preview
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize(m_pageSize));
    printer.setPageOrientation(m_orientation);

    // Create preview dialog
    QPrintPreviewDialog previewDialog(&printer, parent);
    previewDialog.setWindowTitle(m_title.isEmpty() ? "Preview PDF" : "Preview: " + m_title);

    // Connect paint signal
    QObject::connect(&previewDialog, &QPrintPreviewDialog::paintRequested,
                     [&doc](QPrinter *currentPrinter) {
                         doc.print(currentPrinter);
                     });

    qDebug() << "PDFExporter: Displaying preview...";

    // Show preview dialog (blocking)
    int result = previewDialog.exec();

    emit previewClosed();

    return (result == QDialog::Accepted);
}

bool PDFExporter::exportToPdf(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "PDFExporter: Empty file path provided";
        return false;
    }

    emit exportStarted();

    // Generate HTML content
    QString htmlContent = generateHtml();

    // Create QTextDocument
    QTextDocument doc;
    doc.setHtml(htmlContent);

    // Setup printer for PDF export
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(m_pageSize));
    printer.setPageOrientation(m_orientation);

    // Print to PDF
    doc.print(&printer);

    qDebug() << "PDFExporter: PDF exported to" << filePath;

    emit exportFinished(true, filePath);

    return true;
}

bool PDFExporter::previewAndExport(QWidget *parent)
{
    // Generate HTML content once
    QString htmlContent = generateHtml();

    // Create QTextDocument
    QTextDocument doc;
    doc.setHtml(htmlContent);

    // Setup printer for preview
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize(m_pageSize));
    printer.setPageOrientation(m_orientation);

    // Create preview dialog
    QPrintPreviewDialog previewDialog(&printer, parent);
    previewDialog.setWindowTitle(m_title.isEmpty() ? "Preview & Export PDF" : "Preview: " + m_title);

    // Connect paint signal
    QObject::connect(&previewDialog, &QPrintPreviewDialog::paintRequested,
                     [&doc](QPrinter *currentPrinter) {
                         doc.print(currentPrinter);
                     });

    qDebug() << "PDFExporter: Displaying preview...";

    // Show preview dialog
    int result = previewDialog.exec();

    emit previewClosed();

    // If user closed preview, ask to save
    if (result == QDialog::Accepted || result == QDialog::Rejected) {
        QString filePath = QFileDialog::getSaveFileName(
            parent,
            "Simpan PDF",
            QDir::homePath() + "/" + (m_title.isEmpty() ? "document" : m_title) + ".pdf",
            "PDF Files (*.pdf)"
            );

        if (!filePath.isEmpty()) {
            return exportToPdf(filePath);
        }
    }

    return false;
}

QString PDFExporter::getGeneratedHtml() const
{
    return m_lastGeneratedHtml;
}

void PDFExporter::clearData()
{
    m_title.clear();
    m_headers.clear();
    m_data.clear();
    m_customHtml.clear();
    m_prefixHtml.clear();
    m_suffixHtml.clear();
    m_lastGeneratedHtml.clear();
}
