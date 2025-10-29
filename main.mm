#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <AppKit/AppKit.h>

/**** beberapa method untuk kustomisasi warna dari titlebar khusus untuk macos ****/
double gammaCorrect(double c) {
    c /= 255.0;
    return (c <= 0.03928) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
}

bool isDark(QColor color) {
    double luminance = 0.2126 * gammaCorrect(color.red()) +
                       0.7152 * gammaCorrect(color.green()) +
                       0.0722 * gammaCorrect(color.blue());
    return luminance < 0.5;
}

void setCustomizedTitleBar(WId i, QColor clrInHexFromWidgetbG){
      NSView* viewDiag = (NSView*)i;
      qDebug() << Q_FUNC_INFO << viewDiag;
      NSWindow* windowDiag = [viewDiag window];
      windowDiag.titlebarAppearsTransparent = YES;

      windowDiag.backgroundColor = [NSColor colorWithRed: ((double)clrInHexFromWidgetbG.red()/255.0) green: ((double)clrInHexFromWidgetbG.green()/255.0) blue: ((double)clrInHexFromWidgetbG.blue()/255.0) alpha:1.];
      windowDiag.appearance = [NSAppearance appearanceNamed: (isDark(clrInHexFromWidgetbG))?NSAppearanceNameVibrantDark:NSAppearanceNameVibrantLight];
}

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

    //apply custom title bar color in macOS
    QColor clrInHexFromWidgetbG = QColor::fromString("#19232d"); //replace this color in hex with default QWidget background-color from applied theme (in css file)

    QObject::connect(&w, &MainWindow::dialogWinId, [&](WId i){
        setCustomizedTitleBar(i, clrInHexFromWidgetbG);
    });
    // qDebug() << Q_FUNC_INFO << w.effectiveWinId();

    setCustomizedTitleBar(w.effectiveWinId(), clrInHexFromWidgetbG);

    w.show();
    return a.exec();
}
