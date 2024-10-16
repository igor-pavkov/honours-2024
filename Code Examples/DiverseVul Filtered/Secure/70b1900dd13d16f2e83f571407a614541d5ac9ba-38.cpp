int my_trace(CURL *handle, curl_infotype type,
             unsigned char *data, size_t size,
             void *userp)
{
  struct Configurable *config = (struct Configurable *)userp;
  FILE *output=config->errors;
  const char *text;
  struct timeval tv;
  struct tm *now;
  char timebuf[20];
  time_t secs;

  (void)handle; /* prevent compiler warning */

  tv = cutil_tvnow();
  secs = tv.tv_sec;
  now = localtime(&secs);  /* not multithread safe but we don't care */
  if(config->tracetime)
    snprintf(timebuf, sizeof(timebuf), "%02d:%02d:%02d.%06d ",
             now->tm_hour, now->tm_min, now->tm_sec, tv.tv_usec);
  else
    timebuf[0]=0;

  if(!config->trace_stream) {
    /* open for append */
    if(curlx_strequal("-", config->trace_dump))
      config->trace_stream = stdout;
    else if(curlx_strequal("%", config->trace_dump))
      /* Ok, this is somewhat hackish but we do it undocumented for now */
      config->trace_stream = stderr;
    else {
      config->trace_stream = fopen(config->trace_dump, "w");
      config->trace_fopened = TRUE;
    }
  }

  if(config->trace_stream)
    output = config->trace_stream;

  if(config->tracetype == TRACE_PLAIN) {
    /*
     * This is the trace look that is similar to what libcurl makes on its
     * own.
     */
    static const char * const s_infotype[] = {
      "*", "<", ">", "{", "}", "{", "}"
    };
    size_t i;
    size_t st=0;
    static bool newl = FALSE;
    static bool traced_data = FALSE;

    switch(type) {
    case CURLINFO_HEADER_OUT:
      for(i=0; i<size-1; i++) {
        if(data[i] == '\n') { /* LF */
          if(!newl) {
            fprintf(config->trace_stream, "%s%s ",
                    timebuf, s_infotype[type]);
          }
          fwrite(data+st, i-st+1, 1, config->trace_stream);
          st = i+1;
          newl = FALSE;
        }
      }
      if(!newl)
        fprintf(config->trace_stream, "%s%s ", timebuf, s_infotype[type]);
      fwrite(data+st, i-st+1, 1, config->trace_stream);
      newl = (bool)(size && (data[size-1] != '\n'));
      traced_data = FALSE;
      break;
    case CURLINFO_TEXT:
    case CURLINFO_HEADER_IN:
      if(!newl)
        fprintf(config->trace_stream, "%s%s ", timebuf, s_infotype[type]);
      fwrite(data, size, 1, config->trace_stream);
      newl = (bool)(size && (data[size-1] != '\n'));
      traced_data = FALSE;
      break;
    case CURLINFO_DATA_OUT:
    case CURLINFO_DATA_IN:
    case CURLINFO_SSL_DATA_IN:
    case CURLINFO_SSL_DATA_OUT:
      if(!traced_data) {
        /* if the data is output to a tty and we're sending this debug trace
           to stderr or stdout, we don't display the alert about the data not
           being shown as the data _is_ shown then just not via this
           function */
        if(!(config->conf&CONF_ISATTY) ||
           ((config->trace_stream != stderr) &&
            (config->trace_stream != stdout))) {
          if(!newl)
            fprintf(config->trace_stream, "%s%s ", timebuf, s_infotype[type]);
          fprintf(config->trace_stream, "[data not shown]\n");
          newl = FALSE;
          traced_data = TRUE;
        }
      }
      break;
    default: /* nada */
      newl = FALSE;
      traced_data = FALSE;
      break;
    }

    return 0;
  }

#ifdef CURL_DOES_CONVERSIONS
  /* Special processing is needed for CURLINFO_HEADER_OUT blocks
   * if they contain both headers and data (separated by CRLFCRLF).
   * We dump the header text and then switch type to CURLINFO_DATA_OUT.
   */
  if((type == CURLINFO_HEADER_OUT) && (size > 4)) {
    size_t i;
    for(i = 0; i < size - 4; i++) {
      if(memcmp(&data[i], "\r\n\r\n", 4) == 0) {
        /* dump everthing through the CRLFCRLF as a sent header */
        text = "=> Send header";
        dump(timebuf, text, output, data, i+4, config->tracetype, type);
        data += i + 3;
        size -= i + 4;
        type = CURLINFO_DATA_OUT;
        data += 1;
        break;
      }
    }
  }
#endif /* CURL_DOES_CONVERSIONS */

  switch (type) {
  case CURLINFO_TEXT:
    fprintf(output, "%s== Info: %s", timebuf, data);
  default: /* in case a new one is introduced to shock us */
    return 0;

  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  }

  dump(timebuf, text, output, data, size, config->tracetype, type);
  return 0;
}