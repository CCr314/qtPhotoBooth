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
