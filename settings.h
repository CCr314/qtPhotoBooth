#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

#include "params.h"
#include "cups_interface.h"

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

    void on_chkPreview_stateChanged(int arg1);



private:
    Ui::Settings *ui;
    cups_interface cups;
};

#endif // SETTINGS_H
