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


QString cups_interface::statutImprimante()
{
    int		i, j;			/* Looping vars */
    ipp_t		*request,		/* IPP Request */
          *response;		/* IPP Response */
    ipp_attribute_t *attr;
     cups_dest_t *dests;
    QString message="Prête à imprimer";
    int code_erreur;

     static const char *pattrs[] =		/* Attributes we need for printers... */
           {
             "printer-name",
             "printer-state",
             "printer-state-message",
             "printer-state-reasons",
             "printer-state-change-time",
             "printer-type",
             "printer-info",
             "printer-location",
             "printer-make-and-model",
             "printer-uri-supported",
             "requesting-user-name-allowed",
             "requesting-user-name-denied"
           };



    _cupsSetLocale(nullptr);
    dests       = NULL;

    if ((dests = cupsGetNamedDest(CUPS_HTTP_DEFAULT, nullptr, nullptr)) != nullptr)
    {
          _cups_globals_t *cg = _cupsGlobals();	/* Global data */
          request = ippNewRequest(CUPS_GET_PRINTERS);
          ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                        "requested-attributes", sizeof(pattrs) / sizeof(pattrs[0]),
                NULL, pattrs);

          ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
                       NULL, cupsUser());

          if (cupsLastError() == IPP_STATUS_ERROR_BAD_REQUEST ||
              cupsLastError() == IPP_STATUS_ERROR_VERSION_NOT_SUPPORTED)
          {

              message="error - add '/version=1.1' to server name.";
              return message;

          }
          else if (cupsLastError() > IPP_STATUS_OK_CONFLICTING)
          {

              message = QString::fromLocal8Bit(cupsLastErrorString());
              return message;

          }
          response = cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/");
          if (response)
          {
              for (attr = response->attrs; attr != NULL; attr = attr->next)
              {

                  // TODO comment s'assurer que c'est sur la bonne imprimante ?
                    if(attr->group_tag == IPP_TAG_PRINTER)
                    {
                        if (!strcmp(attr->name, "printer-state-message") &&
                                     attr->value_tag == IPP_TAG_TEXT)
                        {
                              message = QString::fromLocal8Bit(attr->values[0].string.text);
                              break;
                        }
                        if(!strcmp(attr->name, "printer-state") && attr->value_tag==IPP_TAG_ENUM) code_erreur=attr->values[0].integer;
                    }

            }
          }
    }

    return message;
}
