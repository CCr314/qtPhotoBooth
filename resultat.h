#ifndef RESULTAT_H
#define RESULTAT_H

#include <QDialog>
#include <QLabel>
#include "params.h"

namespace Ui {
class Resultat;
}

class Resultat : public QDialog
{
    Q_OBJECT

public:
    explicit Resultat(QImage img, QWidget *parent = nullptr);
    ~Resultat();
private slots:
    void imprimer(QMouseEvent *ev);
    void recommencer(QMouseEvent *ev);
    void on_btnPrint_clicked();

    void on_btnRetry_clicked();

private:
    Ui::Resultat *ui;

    void aspectRatio(QLabel*,QPixmap);
};

#endif // RESULTAT_H
