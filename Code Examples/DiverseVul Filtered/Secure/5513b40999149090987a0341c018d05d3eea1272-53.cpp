regerror (int errcode, const regex_t *_Restrict_ preg,
	  char *_Restrict_ errbuf, size_t errbuf_size)
#endif
{
  const char *msg;
  size_t msg_size;

  if (BE (errcode < 0
	  || errcode >= (int) (sizeof (__re_error_msgid_idx)
			       / sizeof (__re_error_msgid_idx[0])), 0))
    /* Only error codes returned by the rest of the code should be passed
       to this routine.  If we are given anything else, or if other regex
       code generates an invalid error code, then the program has a bug.
       Dump core so we can fix it.  */
    abort ();

  msg = gettext (__re_error_msgid + __re_error_msgid_idx[errcode]);

  msg_size = strlen (msg) + 1; /* Includes the null.  */

  if (BE (errbuf_size != 0, 1))
    {
      size_t cpy_size = msg_size;
      if (BE (msg_size > errbuf_size, 0))
	{
	  cpy_size = errbuf_size - 1;
	  errbuf[cpy_size] = '\0';
	}
      memcpy (errbuf, msg, cpy_size);
    }

  return msg_size;
}