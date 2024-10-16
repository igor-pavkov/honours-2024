void write_email(

  FILE      *outmail_input,
  job       *pjob,
  char      *mailto,
  int        mailpoint,
  char      *text)

  {
  char *bodyfmt = NULL;
  char  bodyfmtbuf[MAXLINE];
  char *subjectfmt = NULL;

  /* Pipe in mail headers: To: and Subject: */
  fprintf(outmail_input, "To: %s\n", mailto);

  /* mail subject line formating statement */
  if ((server.sv_attr[SRV_ATR_MailSubjectFmt].at_flags & ATR_VFLAG_SET) &&
      (server.sv_attr[SRV_ATR_MailSubjectFmt].at_val.at_str != NULL))
    {
    subjectfmt = server.sv_attr[SRV_ATR_MailSubjectFmt].at_val.at_str;
    }
  else
    {
    subjectfmt = "PBS JOB %i";
    }

  fprintf(outmail_input, "Subject: ");
  svr_format_job(outmail_input, pjob, subjectfmt, mailpoint, text);
  fprintf(outmail_input, "\n");

  /* Set "Precedence: bulk" to avoid vacation messages, etc */
  fprintf(outmail_input, "Precedence: bulk\n\n");

  /* mail body formating statement */
  if ((server.sv_attr[SRV_ATR_MailBodyFmt].at_flags & ATR_VFLAG_SET) &&
      (server.sv_attr[SRV_ATR_MailBodyFmt].at_val.at_str != NULL))
    {
    bodyfmt = server.sv_attr[SRV_ATR_MailBodyFmt].at_val.at_str;
    }
  else
    {
    bodyfmt =  strcpy(bodyfmtbuf, "PBS Job Id: %i\n"
                                  "Job Name:   %j\n");
    if (pjob->ji_wattr[JOB_ATR_exec_host].at_flags & ATR_VFLAG_SET)
      {
      strcat(bodyfmt, "Exec host:  %h\n");
      }

    strcat(bodyfmt, "%m\n");

    if (text != NULL)
      {
      strcat(bodyfmt, "%d\n");
      }
    }

  /* Now pipe in the email body */
  svr_format_job(outmail_input, pjob, bodyfmt, mailpoint, text);
  } /* write_email() */