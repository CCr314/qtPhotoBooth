#include "commandes.h"
#include "ui_commandes.h"

#include <qtimer.h>
#include <QDebug>
#include <QMouseEvent>

Commandes::Commandes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Commandes)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Commandes::updateTempo);

    ui->label->clear();
    //ui->label->setScaledContents(true);
    setMode(start);

}

Commandes::~Commandes()
{
    delete ui;
}

void Commandes::setMode(modeFonction mode)
{
    switch (mode) {
    case start: ui->label->setText("Démarrage"); break;
    case wait: ui->label->clear(); aspectRatio(ui->label,QPixmap(":/images/takePhoto.png","PNG")); break;
    case smile: ui->label->clear(); aspectRatio(ui->label,QPixmap(":/images/Smiley.png","PNG")); break;
    case traitement: ui->label->clear(); break;
    default : ui->label->setText("Non defini"); break;

    }
}

void Commandes::on_btnStart_clicked()
{
    currentSec=4;
    updateTempo();

    timer->start(1000);
}

void Commandes::updateTempo()
{
    currentSec=currentSec-1;
    ui->label->setNum(currentSec);
    if(currentSec==0)  setMode(smile);

    if(currentSec==-1)
    {
        setMode(traitement);
        emit takePhoto();
        timer->stop();
    }
}

void Commandes::mouseDoubleClickEvent(QMouseEvent *e)
{
    qDebug() << "dblClick";

    if(e->pos().x() < 100 && e->pos().y() < 100)
    {
            qDebug() << "mode Param";
    }

}

void Commandes::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "Click";
    on_btnStart_clicked();

}

void Commandes::aspectRatio(QLabel *label,QPixmap pixmap)
{
    int w = label->width();
    int h = label->height();

    // set a scaled pixmap to a w x h window keeping its aspect ratio
    label->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
}
