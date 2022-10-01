#ifndef PARAMS_H
#define PARAMS_H

#include <QColor>
#include <QSettings>
#include <QPixmap>
#include <QPainter>

class params
{
private:
    params();
    inline static QSettings * settings;
    QColor colotheme;
    inline static QSize screenSize;
    inline static QSize cameraSize;
    inline static QSize imprSize;

    inline static int screenOffsetWidth;
    inline static int screenOffsetHeight;


    inline static QString imagePath;
    inline static QString masquePath;
    inline static bool modePreview;
    inline static int compteur;
    inline static QColor colorTheme;
    inline static QColor colorThemeAlpha;


public:
    static params &GetInstance(){
           static params instance;
           return instance;
        }

    static QSize getScreenSize();
    static QSize getCameraSize();
    static QSize getImprSize();

    static int getScreenOffsetWidth();
    static int getScreenOffsetHeight();

    static QString getImagePath();
    static QString getMasquePath();
    static bool isModePreview();
    static int getCompteur();
    static QColor getColorTheme();
    static void setColorTheme(QColor value);

    static void init();

    static QPixmap getPixmapTheme(QString ressource);
    static QPixmap getPixmapTheme(QString ressource,const char * );
    static QPixmap getPixmapThemeAlpha(QString ressource);
    static QPixmap getPixmapThemeAlpha(QString ressource,const char * );
};

#endif // PARAMS_H
