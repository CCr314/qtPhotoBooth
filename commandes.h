#ifndef COMMANDES_H
#define COMMANDES_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include "params.h"

namespace Ui {
class Commandes;
}

class Commandes : public QDialog
{
    Q_OBJECT

public:
    explicit Commandes(QWidget *parent = nullptr);
    ~Commandes();
    enum modeFonction
    {
        start,
        wait,
        smile,
        traitement,
        print,
    };

    void setMode(modeFonction mode);

private slots:
    void on_btnStart_clicked();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void init();
signals:
    void takePhoto();

private:
    Ui::Commandes *ui;
    int currentSec;
    int defaultsec;

    void updateTempo();

    void aspectRatio(QLabel*,QPixmap);

    QTimer *timer;
    int offset;
};

#endif // COMMANDES_H
