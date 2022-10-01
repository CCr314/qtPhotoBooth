#include "commandes.h"
#include "ui_commandes.h"

#include <qtimer.h>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include "settings.h"

#define TRANSPARENCECOTE "background-color:rgba(255,255,255, 0.4);"

Commandes::Commandes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Commandes)
{
    ui->setupUi(this);

    defaultsec = params::getCompteur()+1;
    setAttribute(Qt::WA_TranslucentBackground);

    this->setGeometry(0,0,params::getScreenSize().width(),params::getScreenSize().height());
    this->showFullScreen();
    this->setModal(false);
    qApp->processEvents();

    offset = params::getScreenOffsetWidth()/2;

    ui->label->setGeometry(offset,0,params::getScreenSize().width()-2*offset,params::getScreenSize().height());
    setMode(start);

    ui->frameDroit->setStyleSheet(TRANSPARENCECOTE) ;
    ui->frameDroit->setGeometry(params::getScreenSize().width()-offset,0,offset,params::getScreenSize().height());

    ui->frameGauche->setStyleSheet(TRANSPARENCECOTE) ;
    ui->frameGauche->setGeometry(0,0,offset,params::getScreenSize().height());


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Commandes::updateTempo);
    connect(this,&Commandes::open,this,&Commandes::init);
}

Commandes::~Commandes()
{
    delete ui;
}

void Commandes::init()
{
    ui->label->clear();
    setMode(start);
}

void Commandes::setMode(modeFonction mode)
{
    switch (mode) {
    case start: ui->label->clear(); aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/photoBoots.png")); break;
    case wait: ui->label->clear(); aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/takePhoto.png")); break;
    case smile: ui->label->clear(); aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/Smiley.png")); break;
    case traitement: ui->label->clear(); break;
    default : ui->label->setText("Non defini"); break;

    }
}

void Commandes::on_btnStart_clicked()
{
    currentSec=defaultsec;
    updateTempo();

    timer->start(1000);
}

void Commandes::updateTempo()
{
    currentSec=currentSec-1;
    switch (currentSec) {
        case -1:setMode(traitement); timer->stop(); emit takePhoto(); break;
        case 0:setMode(smile); break;
        case 1: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/1.png")); break;
        case 2: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/2.png")); break;
        case 3: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/3.png")); break;
        case 4: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/4.png")); break;
        case 5: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/5.png")); break;
        case 6: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/6.png")); break;
        case 7: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/7.png")); break;
        case 8: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/8.png")); break;
        case 9: aspectRatio(ui->label,params::getPixmapThemeAlpha(":/images/9.png")); break;
    }

}

void Commandes::mouseDoubleClickEvent(QMouseEvent *e)
{


}

void Commandes::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "Click";
    if(e->pos().x() > offset )
    {
        on_btnStart_clicked();
    }
    else {
            qDebug() << "mode Param";
            Settings settingsDialog(this);
            settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

            //settingsDialog.setImageSettings(m_imageSettings);

            if (settingsDialog.exec()) {
                //m_imageSettings = settingsDialog.imageSettings();
                //m_imageCapture->setEncodingSettings(m_imageSettings);
            }
        }

}



void Commandes::aspectRatio(QLabel *label,QPixmap pixmap)
{
    int w = label->width()*.8;   // on ne prend que 0ù de l'écran
    int h = label->height()*.8;

    //ui->label->setPixmap(new_image); // showing result

    // set a scaled pixmap to a w x h window keeping its aspect ratio
    label->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
}
