#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

void checkForHomeDir(){
    QString dirStr  = QString("%1/%2").arg(QDir::homePath()).arg(AppEnv::APP_HOMEDIR_NAME);
    qDebug() << Q_FUNC_INFO << dirStr;
    QDir dr(dirStr);

    if (!dr.exists()){
        qDebug() << Q_FUNC_INFO << "the home directory not exist, proceed to make it one";
        dr.mkpath(dirStr);
    } else {
        qDebug() << Q_FUNC_INFO << "the home directory already there, so no need to make it";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    checkForHomeDir();

    //apply css
    QFile fCss(":/qdarkstyle/dark/darkstyle.qss");
    if (fCss.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(fCss.readAll());
        // 2. Terapkan Stylesheet ke aplikasi
        a.setStyleSheet(styleSheet);
        fCss.close();
    } else {
        qWarning("Gagal memuat style.qss!");
    }

    MainWindow w;

    //19232d

    w.show();
    return a.exec();
}
