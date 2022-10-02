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
    int		i, j;			/* Looping vars */
    ipp_t		*request,		/* IPP Request */
          *response,		/* IPP Response */
          *jobs;			/* IPP Get Jobs response */
    ipp_attribute_t *attr,		/* Current attribute */
          *jobattr,		/* Job ID attribute */
          *reasons;		/* Job state reasons attribute */
    const char	*message,		/* Printer state message */
          *description,		/* Description of printer */
          *location,		/* Location of printer */
          *make_model,		/* Make and model of printer */
          *uri;			/* URI of printer */
    ipp_attribute_t *allowed,		/* requesting-user-name-allowed */
          *denied;		/* requestint-user-name-denied */
    ipp_pstate_t	pstate;			/* Printer state */
    cups_ptype_t	ptype;			/* Printer type */
    time_t	ptime;			/* Printer state time */
    int		jobid;			/* Job ID of current job */
    char		printer_uri[HTTP_MAX_URI],
                      /* Printer URI */
          printer_state_time[255];/* Printer state time */
     cups_dest_t **dests;

    char * name="Canon_SELPHY_CP1300";
    static const char *pattrs[] =		/* Attributes we need for printers... */
          {
            "printer-name",
            "printer-state-change-time",
            "printer-state-message",
            "printer-is-accepting-jobs"
          };

    _cupsSetLocale(nullptr);

    char		*pptr,			/* Pointer into printer */
          printer[1024];		/* Current printer/class name */

    strlcpy(printer, name, sizeof(printer));
    if ((pptr = strchr(printer, '/')) != NULL)
      *pptr++ = '\0';

    if ((*dests = cupsGetNamedDest(CUPS_HTTP_DEFAULT, printer, pptr)) == NULL)
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
             // erreur
             // _cupsLangPrintf(stderr,
             //         _("%s: Error - add '/version=1.1' to server name."),
             //         "lpstat");
             // ippDelete(response);
             // return (1);
          }
          else if (cupsLastError() > IPP_STATUS_OK_CONFLICTING)
          {
            //_cupsLangPrintf(stderr, "lpstat: %s", cupsLastErrorString());
            //ippDelete(response);
            //return (1);
          }
          if (response)
          {

          }
    }

}
