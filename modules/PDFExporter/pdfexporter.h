#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPageSize>
#include <functional>

class PDFExporter : public QObject
{
    Q_OBJECT
public:
    explicit PDFExporter(QObject *parent = nullptr);
    ~PDFExporter();

    // Configuration methods
    void setTitle(const QString &title);
    void setShowDate(bool show);
    void setDateFormat(const QString &format);
    void setPageSize(QPageSize::PageSizeId pageSize);
    void setPageOrientation(QPageLayout::Orientation orientation);

    // Table-based export
    void setTableHeaders(const QStringList &headers);
    void setTableData(const QList<QStringList> &data);
    void setHeaderColor(const QString &color);
    void setZebraColors(const QString &evenColor, const QString &oddColor);

    // Custom HTML export
    void setCustomHtml(const QString &html);

    // Additional content
    void setPrefixHtml(const QString &html);  // Content before table
    void setSuffixHtml(const QString &html);  // Content after table

    // Export methods
    bool preview(QWidget *parent = nullptr);
    bool exportToPdf(const QString &filePath);
    bool previewAndExport(QWidget *parent = nullptr);

    // Utility
    QString getGeneratedHtml() const;
    void clearData();

signals:
    void exportStarted();
    void exportFinished(bool success, const QString &filePath);
    void previewClosed();

private:
    // HTML generation
    QString generateHtml();
    QString generateTableHtml();

    // Data members
    QString m_title;
    bool m_showDate;
    QString m_dateFormat;
    QPageSize::PageSizeId m_pageSize;
    QPageLayout::Orientation m_orientation;

    QStringList m_headers;
    QList<QStringList> m_data;

    QString m_headerColor;
    QString m_zebraEvenColor;
    QString m_zebraOddColor;

    QString m_customHtml;
    QString m_prefixHtml;
    QString m_suffixHtml;

    QString m_lastGeneratedHtml;

    // Helper methods
    QString escapeHtml(const QString &text) const;
};

#endif // PDFEXPORTER_H
