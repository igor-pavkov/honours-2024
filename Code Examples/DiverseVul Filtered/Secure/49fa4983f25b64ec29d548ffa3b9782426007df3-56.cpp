validate_user(cupsd_job_t    *job,	/* I - Job */
              cupsd_client_t *con,	/* I - Client connection */
              const char     *owner,	/* I - Owner of job/resource */
              char           *username,	/* O - Authenticated username */
	      size_t         userlen)	/* I - Length of username */
{
  cupsd_printer_t	*printer;	/* Printer for job */


  cupsdLogMessage(CUPSD_LOG_DEBUG2, "validate_user(job=%d, con=%d, owner=\"%s\", username=%p, userlen=" CUPS_LLFMT ")", job->id, con ? con->number : 0, owner ? owner : "(null)", username, CUPS_LLCAST userlen);

 /*
  * Validate input...
  */

  if (!con || !owner || !username || userlen <= 0)
    return (0);

 /*
  * Get the best authenticated username that is available.
  */

  strlcpy(username, get_username(con), userlen);

 /*
  * Check the username against the owner...
  */

  printer = cupsdFindDest(job->dest);

  return (cupsdCheckPolicy(printer ? printer->op_policy_ptr : DefaultPolicyPtr,
                           con, owner) == HTTP_OK);
}