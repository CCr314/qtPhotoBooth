#include "resultat.h"
#include "ui_resultat.h"

#include "QDebug"
#include <QImage>
#define BUTTONWIDTH 120

Resultat::Resultat(QImage img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resultat)
{

    ui->setupUi(this);
    this->setGeometry(0,0,params::getScreenSize().width(),params::getScreenSize().height());
    ui->label->setGeometry(0,0,params::getScreenSize().width()-BUTTONWIDTH,params::getScreenSize().height());
    showFullScreen();

    ui->frame->setGeometry(params::getScreenSize().width()-BUTTONWIDTH,0,BUTTONWIDTH,params::getScreenSize().height());


    ui->btnPrint->setIcon(params::getPixmapTheme(":/images/imprimer2.png"));
    ui->btnRetry->setIcon(params::getPixmapTheme(":/images/retry2.png"));
    //qApp->processEvents();

    this->aspectRatio(ui->label,QPixmap::fromImage(img));
/*
    // gestion de la liste des masques
    ui->listWidget->setGeometry(0,0,BUTTONWIDTH,2*BUTTONWIDTH);
    ui->listWidget->setViewMode  (QListWidget::IconMode);
    this->ui->listWidget->setIconSize(QSize(.8*BUTTONWIDTH, .8*BUTTONWIDTH/1.5));
    this->ui->listWidget->setResizeMode(QListWidget::Adjust);
    QPixmap masque(params::getMasquePath()+"/fond.png","PNG");
    QListWidgetItem *aitem = new QListWidgetItem(masque,"masque defaut");
    ui->listWidget->addItem(aitem);
    */

}

Resultat::~Resultat()
{
    delete ui;
}

void Resultat::aspectRatio(QLabel *label,QPixmap pixmap)
{
    int w = label->width();   // on ne prend que 0ù de l'écran
    int h = label->height();

    // set a scaled pixmap to a w x h window keeping its aspect ratio
    label->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));

}

void Resultat::imprimer(QMouseEvent *ev)
{
    this->done(true);
}
void Resultat::recommencer(QMouseEvent *ev)
{
    this->done(false);
}

void Resultat::on_btnPrint_clicked()
{
    this->done(true);
}

void Resultat::on_btnRetry_clicked()
{
    this->done(false);
}
