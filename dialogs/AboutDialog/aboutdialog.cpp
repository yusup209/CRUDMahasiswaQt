#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lblAppTitle->setText(QString(APP_NAME));
    ui->lblAppVersion->setText(QString("Version %1").arg(APP_VERSION));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
