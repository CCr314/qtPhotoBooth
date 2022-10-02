#include "cups_interface.h"

cups_interface::cups_interface()
{


}


bool cups_interface::serveurOK()
{
    http_t	*http;			/* Connection to server */
    bool cnxOK;

    if ((http = httpConnectEncrypt(cupsServer(), ippPort(),
                                   cupsEncryption())) != NULL)
    {
      cnxOK=true;
      httpClose(http);
    }
    else
      cnxOK=false;

return cnxOK;
}


QString cups_interface::imprimateDefaut()
{
    QString imprName;

    int		num_dests;		/* Number of user destinations */
    cups_dest_t	*dests;			/* User destinations */

    _cupsSetLocale(nullptr);
    num_dests   = 0;
    dests       = NULL;

    cupsFreeDests(num_dests, dests);

    dests     = cupsGetNamedDest(CUPS_HTTP_DEFAULT, NULL, NULL);
    num_dests = dests ? 1 : 0;
    if(num_dests==1)
    {
        imprName = QString::fromLocal8Bit(dests[0].name);
    }
    else {
        imprName = "Pas d'imprimante par defaut";
    }
    return imprName;

}


bool cups_interface::statutImprimante()
{

}
