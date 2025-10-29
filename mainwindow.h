#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QMessageBox>
#include <QDir>
#include "helpers/Environments.h"
#include "helpers/databasemanager.h"
#include "models/tablemodel.h"
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QValidator>
#include <QFileDialog>
#include "dialogs/AboutDialog/aboutdialog.h"
#include "modules/CSVExporter/csvexporter.h"
#include "modules/PDFExporter/pdfexporter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setEnableControls(bool enable = false);
    void setTableColumns();
    void loadStudentsData();

    void clearData();

    bool confirmationMsgBox(const QString &title, const QString &desc);

    void addANewStudent();
    void updateSelectedStudent();
    void deleteSelectedStudent();

    void showReportPreview();
    void exportToPDF();

    void previewDatabaseReport(const QString &reportTitle,
                               const QStringList &headers,
                               const QList<StudentsDataStruct> &data);


    int appMessageBox(const QMessageBox::Icon &msgBoxType, const QString &msgBoxTitle, const QString &msgBoxDesc);

    void exportDataToCSV();

    void on_pushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_lineEdit_4_textChanged(const QString &arg1);

    void on_pushButton_4_clicked();

    void on_actionAbout_this_app_triggered();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<DatabaseManager> dbManager;
    QScopedPointer<TableModel> tblModel;
    QScopedPointer<QSortFilterProxyModel> proxModel;

    int selectedStudentID = -1;
    QScopedPointer<QValidator> npmValidator;


signals:
    void dialogWinId(WId i);

};
#endif // MAINWINDOW_H
