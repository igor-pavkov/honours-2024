cupsdWriteClient(cupsd_client_t *con)	/* I - Client connection */
{
  int		bytes,			/* Number of bytes written */
		field_col;		/* Current column */
  char		*bufptr,		/* Pointer into buffer */
		*bufend;		/* Pointer to end of buffer */
  ipp_state_t	ipp_state;		/* IPP state value */


  cupsdLogClient(con, CUPSD_LOG_DEBUG, "con->http=%p", con->http);
  cupsdLogClient(con, CUPSD_LOG_DEBUG,
		 "cupsdWriteClient "
		 "error=%d, "
		 "used=%d, "
		 "state=%s, "
		 "data_encoding=HTTP_ENCODING_%s, "
		 "data_remaining=" CUPS_LLFMT ", "
		 "response=%p(%s), "
		 "pipe_pid=%d, "
		 "file=%d",
		 httpError(con->http), (int)httpGetReady(con->http),
		 httpStateString(httpGetState(con->http)),
		 httpIsChunked(con->http) ? "CHUNKED" : "LENGTH",
		 CUPS_LLCAST httpGetLength2(con->http),
		 con->response,
		 con->response ? ippStateString(ippGetState(con->request)) : "",
		 con->pipe_pid, con->file);

  if (httpGetState(con->http) != HTTP_STATE_GET_SEND &&
      httpGetState(con->http) != HTTP_STATE_POST_SEND)
  {
   /*
    * If we get called in the wrong state, then something went wrong with the
    * connection and we need to shut it down...
    */

    cupsdLogClient(con, CUPSD_LOG_DEBUG, "Closing on unexpected HTTP write state %s.",
		   httpStateString(httpGetState(con->http)));
    cupsdCloseClient(con);
    return;
  }

  if (con->pipe_pid)
  {
   /*
    * Make sure we select on the CGI output...
    */

    cupsdAddSelect(con->file, (cupsd_selfunc_t)write_pipe, NULL, con);

    cupsdLogClient(con, CUPSD_LOG_DEBUG, "Waiting for CGI data.");

    if (!con->file_ready)
    {
     /*
      * Try again later when there is CGI output available...
      */

      cupsdRemoveSelect(httpGetFd(con->http));
      return;
    }

    con->file_ready = 0;
  }

  bytes = (ssize_t)(sizeof(con->header) - (size_t)con->header_used);

  if (!con->pipe_pid && bytes > (ssize_t)httpGetRemaining(con->http))
  {
   /*
    * Limit GET bytes to original size of file (STR #3265)...
    */

    bytes = (ssize_t)httpGetRemaining(con->http);
  }

  if (con->response && con->response->state != IPP_STATE_DATA)
  {
    size_t wused = httpGetPending(con->http);	/* Previous write buffer use */

    do
    {
     /*
      * Write a single attribute or the IPP message header...
      */

      ipp_state = ippWrite(con->http, con->response);

     /*
      * If the write buffer has been flushed, stop buffering up attributes...
      */

      if (httpGetPending(con->http) <= wused)
        break;
    }
    while (ipp_state != IPP_STATE_DATA && ipp_state != IPP_STATE_ERROR);

    cupsdLogClient(con, CUPSD_LOG_DEBUG,
                   "Writing IPP response, ipp_state=%s, old "
                   "wused=" CUPS_LLFMT ", new wused=" CUPS_LLFMT,
                   ippStateString(ipp_state),
		   CUPS_LLCAST wused, CUPS_LLCAST httpGetPending(con->http));

    if (httpGetPending(con->http) > 0)
      httpFlushWrite(con->http);

    bytes = ipp_state != IPP_STATE_ERROR &&
	    (con->file >= 0 || ipp_state != IPP_STATE_DATA);

    cupsdLogClient(con, CUPSD_LOG_DEBUG,
                   "bytes=%d, http_state=%d, data_remaining=" CUPS_LLFMT,
                   (int)bytes, httpGetState(con->http),
                   CUPS_LLCAST httpGetLength2(con->http));
  }
  else if ((bytes = read(con->file, con->header + con->header_used, (size_t)bytes)) > 0)
  {
    con->header_used += bytes;

    if (con->pipe_pid && !con->got_fields)
    {
     /*
      * Inspect the data for Content-Type and other fields.
      */

      for (bufptr = con->header, bufend = con->header + con->header_used,
               field_col = 0;
           !con->got_fields && bufptr < bufend;
	   bufptr ++)
      {
        if (*bufptr == '\n')
	{
	 /*
	  * Send line to client...
	  */

	  if (bufptr > con->header && bufptr[-1] == '\r')
	    bufptr[-1] = '\0';
	  *bufptr++ = '\0';

          cupsdLogClient(con, CUPSD_LOG_DEBUG, "Script header: %s", con->header);

          if (!con->sent_header)
	  {
	   /*
	    * Handle redirection and CGI status codes...
	    */

	    http_field_t field;		/* HTTP field */
	    char	*value = strchr(con->header, ':');
					/* Value of field */

	    if (value)
	    {
	      *value++ = '\0';
	      while (isspace(*value & 255))
		value ++;
	    }

	    field = httpFieldValue(con->header);

	    if (field != HTTP_FIELD_UNKNOWN && value)
	    {
	      httpSetField(con->http, field, value);

	      if (field == HTTP_FIELD_LOCATION)
	      {
		con->pipe_status = HTTP_STATUS_SEE_OTHER;
		con->sent_header = 2;
	      }
	      else
	        con->sent_header = 1;
	    }
	    else if (!_cups_strcasecmp(con->header, "Status") && value)
	    {
  	      con->pipe_status = (http_status_t)atoi(value);
	      con->sent_header = 2;
	    }
	    else if (!_cups_strcasecmp(con->header, "Set-Cookie") && value)
	    {
	      httpSetCookie(con->http, value);
	      con->sent_header = 1;
	    }
	  }

         /*
	  * Update buffer...
	  */

	  con->header_used -= bufptr - con->header;

	  if (con->header_used > 0)
	    memmove(con->header, bufptr, (size_t)con->header_used);

	  bufptr = con->header - 1;

         /*
	  * See if the line was empty...
	  */

	  if (field_col == 0)
	  {
	    con->got_fields = 1;

	    if (httpGetVersion(con->http) == HTTP_VERSION_1_1 &&
		!httpGetField(con->http, HTTP_FIELD_CONTENT_LENGTH)[0])
	      httpSetLength(con->http, 0);

            cupsdLogClient(con, CUPSD_LOG_DEBUG, "Sending status %d for CGI.", con->pipe_status);

            if (con->pipe_status == HTTP_STATUS_OK)
	    {
	      if (!cupsdSendHeader(con, con->pipe_status, NULL, CUPSD_AUTH_NONE))
	      {
		cupsdCloseClient(con);
		return;
	      }
	    }
	    else
	    {
	      if (!cupsdSendError(con, con->pipe_status, CUPSD_AUTH_NONE))
	      {
		cupsdCloseClient(con);
		return;
	      }
	    }
          }
	  else
	    field_col = 0;
	}
	else if (*bufptr != '\r')
	  field_col ++;
      }

      if (!con->got_fields)
        return;
    }

    if (con->header_used > 0)
    {
      if (httpWrite2(con->http, con->header, (size_t)con->header_used) < 0)
      {
	cupsdLogClient(con, CUPSD_LOG_DEBUG, "Closing for error %d (%s)",
		       httpError(con->http), strerror(httpError(con->http)));
	cupsdCloseClient(con);
	return;
      }

      if (httpIsChunked(con->http))
        httpFlushWrite(con->http);

      con->bytes += con->header_used;

      if (httpGetState(con->http) == HTTP_STATE_WAITING)
	bytes = 0;
      else
        bytes = con->header_used;

      con->header_used = 0;
    }
  }

  if (bytes <= 0 ||
      (httpGetState(con->http) != HTTP_STATE_GET_SEND &&
       httpGetState(con->http) != HTTP_STATE_POST_SEND))
  {
    if (!con->sent_header && con->pipe_pid)
      cupsdSendError(con, HTTP_STATUS_SERVER_ERROR, CUPSD_AUTH_NONE);
    else
    {
      cupsdLogRequest(con, HTTP_STATUS_OK);

      if (httpIsChunked(con->http) && (!con->pipe_pid || con->sent_header > 0))
      {
        cupsdLogClient(con, CUPSD_LOG_DEBUG, "Sending 0-length chunk.");

	if (httpWrite2(con->http, "", 0) < 0)
	{
	  cupsdLogClient(con, CUPSD_LOG_DEBUG, "Closing for error %d (%s)",
			 httpError(con->http), strerror(httpError(con->http)));
	  cupsdCloseClient(con);
	  return;
	}
      }

      cupsdLogClient(con, CUPSD_LOG_DEBUG, "Flushing write buffer.");
      httpFlushWrite(con->http);
      cupsdLogClient(con, CUPSD_LOG_DEBUG, "New state is %s", httpStateString(httpGetState(con->http)));
    }

    cupsdAddSelect(httpGetFd(con->http), (cupsd_selfunc_t)cupsdReadClient, NULL, con);

    cupsdLogClient(con, CUPSD_LOG_DEBUG, "Waiting for request.");

    if (con->file >= 0)
    {
      cupsdRemoveSelect(con->file);

      if (con->pipe_pid)
	cupsdEndProcess(con->pipe_pid, 0);

      close(con->file);
      con->file     = -1;
      con->pipe_pid = 0;
    }

    if (con->filename)
    {
      unlink(con->filename);
      cupsdClearString(&con->filename);
    }

    if (con->request)
    {
      ippDelete(con->request);
      con->request = NULL;
    }

    if (con->response)
    {
      ippDelete(con->response);
      con->response = NULL;
    }

    cupsdClearString(&con->command);
    cupsdClearString(&con->options);
    cupsdClearString(&con->query_string);

    if (!httpGetKeepAlive(con->http))
    {
      cupsdLogClient(con, CUPSD_LOG_DEBUG,
		     "Closing because Keep-Alive is disabled.");
      cupsdCloseClient(con);
      return;
    }
    else
    {
      cupsArrayRemove(ActiveClients, con);
      cupsdSetBusyState();
    }
  }
}