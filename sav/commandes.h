#ifndef COMMANDES_H
#define COMMANDES_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

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

signals:
    void takePhoto();

private:
    Ui::Commandes *ui;
    int currentSec;

    void updateTempo();

    void aspectRatio(QLabel*,QPixmap);

    QTimer *timer;
};

#endif // COMMANDES_H
