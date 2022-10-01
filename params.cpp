#include "params.h"

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

params::params()
{



}

void params::init()
{
    // charge les parametres
    settings=new QSettings(QString("config.ini"),QSettings::IniFormat);
    settings->setValue("version", 1);

    imagePath=settings->value("imagePath","./photos").toString();
    masquePath=settings->value("maskPath","./masque").toString();
    modePreview=settings->value("modePreview","false").toBool();
    compteur=settings->value("compteur","3").toInt();


    int alpha = settings->value("themeCouleur_a","200").toInt();
    if(alpha==0 || alpha == 255) alpha=200;
    colorThemeAlpha = QColor(settings->value("themeCouleur_r","143").toInt(),
                        settings->value("themeCouleur_g","64").toInt(),
                        settings->value("themeCouleur_b","191").toInt(),
                        alpha);

    colorTheme = QColor(settings->value("themeCouleur_r","143").toInt(),
                        settings->value("themeCouleur_g","64").toInt(),
                        settings->value("themeCouleur_b","191").toInt(),
                        255);

    screenSize = QSize(QApplication::desktop()->screenGeometry().width(),QApplication::desktop()->screenGeometry().height());
    //screenSize = QSize(800,480);
    cameraSize = QSize(1920,1080);
    imprSize = QSize(1590,1080);

    // calcule les ratio et offset screen vs imprimante
    float ratiowidth=static_cast<float> (screenSize.width())/static_cast<float> (imprSize.width());
    float ratioheight=static_cast<float> (screenSize.height())/static_cast<float> (imprSize.height());


    if(ratiowidth<ratioheight)
    {
        // offset horizontaux
        ratioheight=ratioheight/ratiowidth;
        screenOffsetWidth=0;
        screenOffsetHeight=screenSize.height()-screenSize.height()/ratioheight;
    }
    else {
        // offset verticaux
        ratiowidth = ratiowidth/ratioheight;
        screenOffsetWidth=screenSize.width()-screenSize.width()/ratiowidth;
        screenOffsetHeight=0;
    }

    qDebug() << "screen " << screenSize.width() << "x" << screenSize.height();
    qDebug() << "camera " << cameraSize.width() << "x" << cameraSize.height();
    qDebug() << "impr " << imprSize.width() << "x" << imprSize.height();
    qDebug() << "ratio " << ratiowidth << "x" << ratioheight;
    qDebug() << "offset " << screenOffsetWidth << "x" << screenOffsetHeight;

}

QSize params::getScreenSize() {return screenSize;}
QSize params::getCameraSize() {return cameraSize;}
QSize params::getImprSize() {return imprSize;}


int params::getScreenOffsetWidth() {return screenOffsetWidth;}
int params::getScreenOffsetHeight() {return screenOffsetHeight;}

QString params::getImagePath() {return imagePath;}

QString params::getMasquePath() {return masquePath;}

bool params::isModePreview() {return modePreview;}

int params::getCompteur() {return compteur;}

QColor params::getColorTheme() {return colorTheme;}

void params::setColorTheme(QColor value)
{
    if(value.alpha()==0 || value.alpha()==255) value.setAlpha(200);

    colorTheme=value;
    colorThemeAlpha=value;

    settings->setValue("themeCouleur_r",colorTheme.red());
    settings->setValue("themeCouleur_g",colorTheme.green());
    settings->setValue("themeCouleur_b",colorTheme.blue());
    settings->setValue("themeCouleur_a",colorTheme.alpha());
}

QPixmap params::getPixmapTheme(QString ressource)
{
    return getPixmapTheme(ressource,"PNG");

}

QPixmap params::getPixmapThemeAlpha(QString ressource)
{
    return getPixmapThemeAlpha(ressource,"PNG");

}

QPixmap params::getPixmapTheme(QString ressource,const char * type)
{
    QPixmap new_image = QPixmap(ressource,type);
    QPainter painter(&new_image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(new_image.rect(), colorTheme);
    painter.end();

    return new_image;
}

QPixmap params::getPixmapThemeAlpha(QString ressource,const char * type)
{
    QPixmap new_image = QPixmap(ressource,type);
    QPainter painter(&new_image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(new_image.rect(), params::colorThemeAlpha);
    painter.end();

    return new_image;
}
