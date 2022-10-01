#ifndef CUPS_INTERFACE_H
#define CUPS_INTERFACE_H

#include <cups/cups-private.h>

class cups_interface
{
public:
    cups_interface();

    bool serveurOK();
};

#endif // CUPS_INTERFACE_H
