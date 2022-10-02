#include "settings.h"
#include "ui_settings.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QDebug>
#include <QStorageInfo>
#include <QProcess>
#include <QColorDialog>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);


    ui->lblImprStatut->setText(cups.imprimateDefaut());
    ui->lblCupsStatus->setText(cups.statutImprimante());

    /*
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            if (!storage.isReadOnly()) {
                 qDebug() << "name:" << storage.name();
                 qDebug() << "fileSystemType:" << storage.fileSystemType();
                 qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
            }
        }
    }

      if (storage.isReadOnly())
          qDebug() << "isReadOnly:" << storage.isReadOnly();

      qDebug() << "name:" << storage.name();
      qDebug() << "fileSystemType:" << storage.fileSystemType();
      qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
      qDebug() << "availableSize:" << storage.bytesAvailable()/1000/1000 << "MB";
*/


}

Settings::~Settings()
{
    delete ui;
}



void Settings::on_btnImprimante_clicked()
{
    //
    //QProcess process;
    //process.start("/usr/bin/chromium-browser http://localhost:631");
    //QProcess::execute("/usr/bin/chromium-browser http://localhost:631");

    QProcess::execute("cupsenable Canon_SELPHY_CP1300");
}

void Settings::on_btnRestart_clicked()
{
    qApp->exit();
}

void Settings::on_btnColor_clicked()
{

   //QColor result = QColorDialog::getColor(params::getColorTheme(), this , "Sélectionnez le thème couleur"); //,QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

   //if(result.isValid()) params::setColorTheme(result);
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(params::getColorTheme());
    dialog->open(this,SLOT(onColorChange(QColor)));

}

void Settings::on_chkPreview_stateChanged(int arg1)
{
    params::setModePreview(ui->chkPreview->checkState());
}


void Settings::onColorChange(QColor value)
{
    params::setColorTheme(value);

}
