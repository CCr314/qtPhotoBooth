#ifndef CUPS_INTERFACE_H
#define CUPS_INTERFACE_H

#include <cups/cups-private.h>
#include <QString>

class cups_interface
{
public:
    cups_interface();

    bool serveurOK();
    QString imprimateDefaut();
    QString statutImprimante();
};

#endif // CUPS_INTERFACE_H
