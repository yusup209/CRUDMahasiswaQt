#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dbManager(new DatabaseManager(QString("%1/%2/%3").arg(QDir::homePath()).arg(AppEnv::APP_HOMEDIR_NAME).arg(AppEnv::APP_DATABASE_NAME)))
    , tblModel(new TableModel(this))
    , proxModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    qDebug() << Q_FUNC_INFO << "welcome to MainWindow class, setting up rxNPM";

    QRegularExpression rxNPM("\\d+");
    npmValidator.reset(new QRegularExpressionValidator(rxNPM, this));

    qDebug() << Q_FUNC_INFO << "checking for database connection....";
    if (!dbManager.get()->isDatabaseOpen()) {
        qCritical() << Q_FUNC_INFO << "Database tidak terbuka. Tidak dapat memuat data.";

        QMessageBox::warning(this, "Fail", "Fail to open database");

        setEnableControls(false);
        return;
    } else {
        qDebug() << Q_FUNC_INFO << "Database sukses terbuka";

        setTableColumns();

        QTimer::singleShot(1750, this, &MainWindow::loadStudentsData);
    }
    qDebug() << Q_FUNC_INFO << "database connection checking passed, setting integer validator to  NPM textbox";

    ui->lineEdit_2->setValidator(npmValidator.get());

    //set search icon to search textbox (lineEdit_4)
    qDebug() << Q_FUNC_INFO << "Setting search icon to search textbox (above table)";
    QIcon icnSearch = QIcon(":/qss_icons/dark/rc/searchIcon_grey.png");
    ui->lineEdit_4->setClearButtonEnabled(true);
    ui->lineEdit_4->addAction(icnSearch, QLineEdit::LeadingPosition);

    qDebug() << Q_FUNC_INFO << "done";
    changeTitlebarColor(winId());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEnableControls(bool enable)
{
    if (!ui->centralwidget) {
        qWarning() << "Error: centralwidget is null.";
        return;
    }

    qDebug() << Q_FUNC_INFO << (enable ? "Enabling" : "Disabling") << "QLineEdit and QPushButton widgets...";

    // --- 1. Iterasi melalui QLineEdit ---

    // QWidget::findChildren<T>() secara rekursif mencari semua turunan dari tipe T
    QList<QLineEdit *> lineEdits = ui->centralwidget->findChildren<QLineEdit *>();

    int lineEditCount = 0;
    for (QLineEdit *lineEdit : lineEdits) {
        lineEdit->setEnabled(enable);
        lineEditCount++;
    }
    qDebug() << Q_FUNC_INFO << "Total QLineEdit affected:" << lineEditCount;


    // --- 2. Iterasi melalui QPushButton ---

    QList<QPushButton *> pushButtons = ui->centralwidget->findChildren<QPushButton *>();

    int buttonCount = 0;
    for (QPushButton *button : pushButtons) {
        button->setEnabled(enable);
        buttonCount++;
    }
    qDebug() << Q_FUNC_INFO << "Total QPushButton affected:" << buttonCount;

    qDebug() << Q_FUNC_INFO << "Widget toggling complete.";
}

void MainWindow::setTableColumns()
{
    QStringList tableColumns = QStringList() << "Nama" << "NPM" << "Kelas";
    tblModel.get()->setColumns(tableColumns);

    proxModel.get()->setSourceModel(tblModel.get());

    ui->tableView->setModel(proxModel.get());
}

void MainWindow::loadStudentsData()
{
    QString tableName =  "mahasiswa";
    QStringList columnsToRetrieve = QStringList() << "id" << "nama" << "npm" << "kelas";
    QList<StudentsDataStruct> queryResult = dbManager->selectRecords(tableName, columnsToRetrieve);

    tblModel.get()->setTableData(queryResult);
}

void MainWindow::clearData()
{
    QList<QLineEdit *> lineEdits = ui->centralwidget->findChildren<QLineEdit *>();

    for (QLineEdit *lineEdit : lineEdits) {
        lineEdit->clear();
    }

    selectedStudentID = -1;
}

bool MainWindow::confirmationMsgBox(const QString &title, const QString &desc)
{
    QMessageBox::StandardButton result = QMessageBox::question(this, title, desc);
    if (result == QMessageBox::StandardButton::Yes){
        return true;
    }

    return false;
}

void MainWindow::addANewStudent()
{
    QString nama = ui->lineEdit->text();
    QString npm = ui->lineEdit_2->text();
    QString kelas = ui->lineEdit_3->text();

    if (nama.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student name");

        appMessageBox(QMessageBox::Warning, "Empty", "Please input the student name");
        return;
    }

    if (npm.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student number (NPM)");
        appMessageBox(QMessageBox::Warning, "Empty", "Please input student number (NPM)");
        return;
    }

    if (kelas.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student class (kelas)");
        appMessageBox(QMessageBox::Warning, "Empty", "Please input student class (kelas)");
        return;
    }

    QString tblNameToInsert = "mahasiswa";
    QVariantMap userData;
    userData["nama"] = nama;
    userData["npm"] = npm;
    userData["kelas"] = kelas;

    qint64 newId = dbManager.get()->insertRecord(tblNameToInsert, userData);
    if (newId > -1) {
        qDebug() << "User baru berhasil ditambahkan dengan ID:" << newId;
        // QMessageBox::information(this, "Success", "New Student data has been added");
        appMessageBox(QMessageBox::Information, "Success", "New Student data has been added");

        QTimer::singleShot(500, this, &MainWindow::loadStudentsData);
        clearData();
    }

}

void MainWindow::updateSelectedStudent()
{
    if (selectedStudentID < 0){
        // QMessageBox::warning(this, "Attention", "You should choose one student record");
        appMessageBox(QMessageBox::Warning, "Attention", "You should choose one student record");
        return;
    }

    QString nama = ui->lineEdit->text();
    QString npm = ui->lineEdit_2->text();
    QString kelas = ui->lineEdit_3->text();

    if (nama.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student name");
        appMessageBox(QMessageBox::Warning, "Empty", "Please input student name");
        return;
    }

    if (npm.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student number (NPM)");
        appMessageBox(QMessageBox::Warning, "Empty", "Please input student number (NPM)");
        return;
    }

    if (kelas.trimmed().length() == 0){
        // QMessageBox::warning(this, "Empty", "Please input student class (kelas)");
        appMessageBox(QMessageBox::Warning, "Empty", "Please input student class (kelas)");
        return;
    }

    QString tableName = "mahasiswa";

    QVariantMap updateData;
    updateData["nama"] = nama;
    updateData["npm"] = npm;
    updateData["kelas"] = kelas;

    QString updateCondition = "id = :patokan_id";
    QVariantMap updateBindValues;
    updateBindValues[":patokan_id"] = QString::number(selectedStudentID);
    updateBindValues[":nama"] = nama;
    updateBindValues[":npm"] = npm;
    updateBindValues[":kelas"] = kelas;

    qDebug() << Q_FUNC_INFO << QString("id: %1 | nama : %2 | npm : %3 | kelas : %4")
                                   .arg(QString::number(selectedStudentID)).arg(nama).arg(npm).arg(kelas);

    if (dbManager.get()->updateRecord(tableName, updateData, updateCondition, updateBindValues)) {
        // QMessageBox::information(this, "Success", "Student data updated");
        appMessageBox(QMessageBox::Information, "Success", "Student data updated");

        QTimer::singleShot(500, this, &MainWindow::loadStudentsData);
        clearData();
    } else {
        // QMessageBox::critical(this, "Failed", "The system fail to update the student data for some reason");
        appMessageBox(QMessageBox::Critical, "Failed", "The system fail to update the student data for some reason");
    }
}

void MainWindow::deleteSelectedStudent()
{
    if (selectedStudentID < 0){
        // QMessageBox::warning(this, "Attention", "You should choose one student record");
        appMessageBox(QMessageBox::Warning, "Attention", "You should choose one student record");
        return;
    }

    QString tableName = "mahasiswa";
    QString deleteCondition = "id = :patokan_id";
    QVariantMap deleteBindValues;
    deleteBindValues[":patokan_id"] = QString::number(selectedStudentID);

    if (dbManager.get()->deleteRecord(tableName, deleteCondition, deleteBindValues)){
        // QMessageBox::information(this, "Success", "Student data deleted");
        appMessageBox(QMessageBox::Information, "Success", "Student data deleted");

        QTimer::singleShot(500, this, &MainWindow::loadStudentsData);
        clearData();
    } else {
        // QMessageBox::critical(this, "Failed", "The system fail to delete the student data for some reason");
        appMessageBox(QMessageBox::Critical, "Failed", "The system fail to delete the student data for some reason");
    }
}

void MainWindow::showReportPreview()
{
    if (!dbManager.get()->isDatabaseOpen()) {
        // QMessageBox::critical(this, "Error", "Database tidak terbuka.");
        appMessageBox(QMessageBox::Critical, "Error", "Database tidak terbuka.");
        return;
    }

    QStringList reportColumns;
    reportColumns << "id" << "nama" << "npm" << "kelas";

    // Ambil Data dari Database
    QList<StudentsDataStruct> reportData = dbManager.get()->selectRecords(
        "mahasiswa",
        reportColumns,
        "", // Semua record
        QVariantMap {}
        );

    if (reportData.isEmpty()) {
        // QMessageBox::information(this, "Info", "Tidak ada data yang tersedia.");
        appMessageBox(QMessageBox::Information, "Info", "Tidak ada data yang tersedia");

        return;
    }

    previewDatabaseReport(
        "Laporan Mahasiswa",
        reportColumns,
        reportData
        );
}

void MainWindow::exportToPDF()
{
    // --- Configuration ---
    // const QString OUTPUT_FILE = "LargeDataReport.pdf";

    // const int DATASET_SIZE = 100; // 5005 rows ensures 11 chunks (500*10 + 5 remaining)

    // const QString REPORT_TITLE = "Quarterly Sales Data Report";
    // const QString REPORT_LABEL = "Generated by Qt Buffered Exporter";
    // const QList<QString> HEADERS = {"Txn ID", "Customer Name", "Amount ($)", "Sale Date"};
    // // -------------------

    // const QString OUTPUT_FILE = QFileDialog::getSaveFileName(this, "Choose where you want to save this PDF file", QDir::homePath(), "PDF File (*.pdf)");

    // if (OUTPUT_FILE.trimmed().isEmpty()){
    //     appMessageBox(QMessageBox::Warning, "Empty", "You should enter file name for exported pdf file");
    //     return;
    // }

    // PDFExporter exporter(this, REPORT_TITLE, REPORT_LABEL, HEADERS, ROWS_PER_CHUNK);

    // qDebug() << "Starting PDF export for" << DATASET_SIZE << "rows (Chunk Size:" << ROWS_PER_CHUNK << ").";

    // QElapsedTimer totalTimer;
    // totalTimer.start();

    // if (exporter.openExport(OUTPUT_FILE)) {
    //     // 1. Generate and write data rows
    //     for (int i = 1; i <= DATASET_SIZE; ++i) {
    //         QList<QVariant> dataRow;
    //         dataRow << QString::number(10000 + i)
    //                 << QString("Customer %1").arg(i)
    //                 << (double)i * 1.23 + 50.0
    //                 << QDateTime::currentDateTime().addDays(i / 10).toString("yyyy-MM-dd");

    //         // This is the call that manages the buffer and triggers flushes
    //         exporter.writeRow(dataRow);
    //     }

    //     // 2. Finalize export (flushes the remaining rows)
    //     exporter.closeExport();
    // } else {
    //     qCritical() << "Export initialization failed!";
    // }

    // qDebug() << "Total operation time:" << totalTimer.elapsed() << "ms.";
}

void MainWindow::previewDatabaseReport(const QString &reportTitle, const QStringList &headers, const QList<StudentsDataStruct> &data)
{
    // // 1. Buat Konten HTML
    // QTextDocument doc;
    // QString htmlContent;

    // // --- Header Laporan ---
    // htmlContent += QString("<h1 style='color:#333333;'>%1</h1>").arg(reportTitle);
    // htmlContent += QString("<p style='color:#333333;'>Tanggal Dibuat: %1</p>").arg(QDateTime::currentDateTime().toString("dd MMMM yyyy, hh:mm:ss"));
    // htmlContent += "<hr/>";

    // // --- Data Tabel (Menggunakan subset HTML/CSS yang didukung QTextDocument) ---
    // htmlContent += "<table width='100%' border='1' cellpadding='5' cellspacing='0'>";

    // // Baris Header Tabel
    // htmlContent += "<tr bgcolor='#D3D3D3'>";
    // for (const QString &header : headers) {
    //     htmlContent += QString("<th>%1</th>").arg(header.toUpper());
    // }
    // htmlContent += "</tr>";

    // // Data Baris (dengan warna zebra)
    // int rowCount = 0;

    // for (StudentsDataStruct rowData : data){
    //     // Iterasi berdasarkan urutan header

    //     QString rowColor = (rowCount % 2 == 0) ? "#FFFFFF" : "#F0F0F0";
    //     htmlContent += QString("<tr bgcolor='%1'>").arg(rowColor);


    //     QString idValue = QString::number(rowData.id);
    //     QString namaValue = rowData.nama;
    //     QString npmValue = rowData.npm;
    //     QString kelasValue = rowData.kelas;

    //     QString tempHtmlContent = QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td>")
    //                                   .arg(idValue).arg(namaValue).arg(npmValue).arg(kelasValue);

    //     htmlContent += tempHtmlContent;
    //     htmlContent += "</tr>";
    //     rowCount++;
    // }

    // htmlContent += "</table>";

    // // 2. Set Konten ke QTextDocument
    // doc.setHtml(htmlContent);

    // // 3. Inisialisasi QPrinter (hanya untuk tampilan layout)
    // QPrinter printer(QPrinter::HighResolution);
    // printer.setPageSize(QPageSize(QPageSize::A4));

    // // 4. Tampilkan Dialog Pratinjau
    // // QPrintPreviewDialog menggunakan QPrinter untuk merender dokumen
    // // ke dalam tampilan preview.
    // QPrintPreviewDialog previewDialog(&printer);

    // // Hubungkan sinyal PaintRequested untuk menggambar dokumen ke QPrinter
    // QObject::connect(&previewDialog, &QPrintPreviewDialog::paintRequested,
    //                  [&doc](QPrinter *currentPrinter) {
    //                      doc.print(currentPrinter);
    //                  });

    // qDebug() << "Menampilkan pratinjau laporan...";

    // // Tampilkan dialog (blokir hingga ditutup)
    // previewDialog.exec();

    // PDFExporter2 exporter; //previously PDFExporter2, now renamed to just PDFExporter
    PDFExporter exporter;

    // Configure exporter
    exporter.setTitle(reportTitle);
    exporter.setTableHeaders(headers);

    // Convert StudentsDataStruct to QList<QStringList>
    QList<QStringList> tableData;
    for (const StudentsDataStruct &student : data) {
        QStringList row;
        row << QString::number(student.id)
            << student.nama
            << student.npm
            << student.kelas;
        tableData.append(row);
    }
    exporter.setTableData(tableData);

    // Preview only
    exporter.preview(this);
}

int MainWindow::appMessageBox(const QMessageBox::Icon &msgBoxType, const QString &msgBoxTitle, const QString &msgBoxDesc)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(msgBoxTitle);
    msgBox.setText(msgBoxDesc);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBoxType == QMessageBox::Information){
        msgBox.setIconPixmap(QIcon(":/sysicon/images/information-freepik-white.png").pixmap(64,64));
    } else if (msgBoxType == QMessageBox::Warning){
        msgBox.setIconPixmap(QIcon(":/sysicon/images/critical-freepik-white.png").pixmap(64,64));
    } else if (msgBoxType == QMessageBox::Critical){
        msgBox.setIconPixmap(QIcon(":/sysicon/images/question-mark-freepik_white.png").pixmap(64,64));
    } else if (msgBoxType == QMessageBox::Question){
        msgBox.setIconPixmap(QIcon(":/sysicon/images/question-mark-freepik_white.png").pixmap(64,64));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
    } else {
        msgBox.setIconPixmap(QIcon(":/sysicon/images/critical-freepik-white.png").pixmap(64,64));
    }

    msgBox.exec();
    return msgBox.result();
}

void MainWindow::exportDataToCSV()
{
    QString completeFilePath = QFileDialog::getSaveFileName(this, "Choose where you want to save this csv file", QDir::homePath(), "CSV File (*.csv)");

    if (completeFilePath.trimmed().isEmpty()){
        appMessageBox(QMessageBox::Warning, "Empty", "You should enter file name for exported csv file");
        return;
    }

    QStringList reportColumns;
    reportColumns << "id" << "nama" << "npm" << "kelas";

    // Ambil Data dari Database
    QVector<QStringList> reportData = dbManager.get()->selectRecordsToVector(
        "mahasiswa",
        reportColumns,
        "", // Semua record
        QVariantMap {}
        );

    if (reportData.isEmpty()) {
        appMessageBox(QMessageBox::Information, "Info", "Tidak ada data yang tersedia");

        return;
    }

    CSVExporter exporter;
    exporter.setDelimiter(";");
    exporter.setFilePath(completeFilePath);

    QElapsedTimer timer;
    timer.start();

    if (exporter.exportDataWithHeaders(reportColumns, reportData))
    {
        qint64 elapsed = timer.elapsed();
        qDebug() << Q_FUNC_INFO << "Small dataset exported in" << elapsed << "ms";

        appMessageBox(QMessageBox::Information, "Success","CSV data exported");
    } else {
        appMessageBox(QMessageBox::Critical, "Failed",  "The System are fail to export the CSV file");
    }
}

void MainWindow::on_pushButton_clicked()
{
    addANewStudent();
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int tblIndex = index.row();
    StudentsDataStruct data = tblModel.get()->getCurrentData(tblIndex);

    selectedStudentID = data.id;
    ui->lineEdit->setText(data.nama);
    ui->lineEdit_2->setText(data.npm);
    ui->lineEdit_3->setText(data.kelas);

    qDebug() << Q_FUNC_INFO << "selectedStudentID : " << selectedStudentID;
    ui->tableView->clearSelection();
}


void MainWindow::on_pushButton_5_clicked()
{
    clearData();
}

void MainWindow::on_pushButton_2_clicked()
{
    // if (confirmationMsgBox("Confirmation","Are you sure you want to update this data?")){
    //     updateSelectedStudent();
    // }

    int confirmationMsgBoxResult = appMessageBox(QMessageBox::Question, "Confirmation", "Are you sure you want to update this data ?");
    if (confirmationMsgBoxResult == QMessageBox::Yes){
        updateSelectedStudent();
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    // if (confirmationMsgBox("Confirmation","Are you sure you want to delete this data?")){
    //     deleteSelectedStudent();
    // }

    int confirmationMsgBoxResult = appMessageBox(QMessageBox::Question,"Confirmation","Are you sure you want to delete this data ?");
    if (confirmationMsgBoxResult == QMessageBox::Yes){
        deleteSelectedStudent();
    }
}


void MainWindow::on_lineEdit_4_textChanged(const QString &arg1)
{
    proxModel.get()->setFilterRegularExpression(arg1);
}


void MainWindow::on_pushButton_4_clicked()
{
    showReportPreview();
    // exportToPDF();
}


void MainWindow::on_actionAbout_this_app_triggered()
{
    AboutDialog abot;
    abot.exec();
}


void MainWindow::on_pushButton_6_clicked()
{
    exportDataToCSV();
}



void MainWindow::changeTitlebarColor(WId winid)
{
    // Get the window handle from Qt window
    //HWND hwnd = (HWND)winId();
    HWND hwnd = (HWND)winid;

    // Make sure the window is using DWM composition
    BOOL dwmEnabled = FALSE;
    HRESULT hr = DwmIsCompositionEnabled(&dwmEnabled);
    if (SUCCEEDED(hr) && dwmEnabled)
    {
        // Define titlebar colors
        COLORREF titlebarColor = RGB(clrInHexFromWidgetbG.red(), clrInHexFromWidgetbG.green(), clrInHexFromWidgetbG.blue());      // Blue background
        qDebug() << Q_FUNC_INFO << clrInHexFromWidgetbG.redF()/255.0;
        COLORREF titleTextColor = isDark(clrInHexFromWidgetbG)?RGB(255,255,255):RGB(0,0,0);    // White text

        // Set title bar color
        DWMWINDOWATTRIBUTE dwAttribute = DWMWA_CAPTION_COLOR;
        DwmSetWindowAttribute(hwnd, dwAttribute, &titlebarColor, sizeof(titlebarColor));

        // Set title text color
        dwAttribute = DWMWA_TEXT_COLOR;
        DwmSetWindowAttribute(hwnd, dwAttribute, &titleTextColor, sizeof(titleTextColor));
    }
}



double MainWindow::gammaCorrect(double c)
{
    c /= 255.0;
    return (c <= 0.03928) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
}



bool MainWindow::isDark(QColor color)
{
    double luminance = 0.2126 * gammaCorrect(color.red()) +
                       0.7152 * gammaCorrect(color.green()) +
                       0.0722 * gammaCorrect(color.blue());
    return luminance < 0.5;
}

