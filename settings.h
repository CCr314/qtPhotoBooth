#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

#include "params.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:

    void on_btnImprimante_clicked();

    void on_btnRestart_clicked();

    void on_btnColor_clicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
