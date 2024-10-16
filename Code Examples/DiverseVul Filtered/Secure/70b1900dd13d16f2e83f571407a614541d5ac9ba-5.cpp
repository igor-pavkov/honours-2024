static int myprogress (void *clientp,
                       double dltotal,
                       double dlnow,
                       double ultotal,
                       double ulnow)
{
  /* The original progress-bar source code was written for curl by Lars Aas,
     and this new edition inherits some of his concepts. */

  char line[256];
  char outline[256];
  char format[40];
  double frac;
  double percent;
  int barwidth;
  int num;
  int i;

  struct ProgressData *bar = (struct ProgressData *)clientp;
  curl_off_t total = (curl_off_t)dltotal + (curl_off_t)ultotal +
    bar->initial_size; /* expected transfer size */
  curl_off_t point = (curl_off_t)dlnow + (curl_off_t)ulnow +
    bar->initial_size; /* we've come this far */

  if(point > total)
    /* we have got more than the expected total! */
    total = point;

  bar->calls++; /* simply count invokes */

  if(total < 1) {
    curl_off_t prevblock = bar->prev / 1024;
    curl_off_t thisblock = point / 1024;
    while ( thisblock > prevblock ) {
      fprintf( bar->out, "#" );
      prevblock++;
    }
  }
  else {
    frac = (double)point / (double)total;
    percent = frac * 100.0f;
    barwidth = bar->width - 7;
    num = (int) (((double)barwidth) * frac);
    i = 0;
    for ( i = 0; i < num; i++ ) {
      line[i] = '#';
    }
    line[i] = '\0';
    snprintf( format, sizeof(format), "%%-%ds %%5.1f%%%%", barwidth );
    snprintf( outline, sizeof(outline), format, line, percent );
    fprintf( bar->out, "\r%s", outline );
  }
  fflush(bar->out);
  bar->prev = point;

  return 0;
}