void progressbarinit(struct ProgressData *bar,
                     struct Configurable *config)
{
#ifdef __EMX__
  /* 20000318 mgs */
  int scr_size [2];
#endif
  char *colp;

  memset(bar, 0, sizeof(struct ProgressData));

  /* pass this through to progress function so
   * it can display progress towards total file
   * not just the part that's left. (21-may-03, dbyron) */
  if (config->use_resume)
    bar->initial_size = config->resume_from;

/* TODO: get terminal width through ansi escapes or something similar.
         try to update width when xterm is resized... - 19990617 larsa */
#ifndef __EMX__
  /* 20000318 mgs
   * OS/2 users most likely won't have this env var set, and besides that
   * we're using our own way to determine screen width */
  colp = curlx_getenv("COLUMNS");
  if (colp != NULL) {
    bar->width = atoi(colp);
    curl_free(colp);
  }
  else
    bar->width = 79;
#else
  /* 20000318 mgs
   * We use this emx library call to get the screen width, and subtract
   * one from what we got in order to avoid a problem with the cursor
   * advancing to the next line if we print a string that is as long as
   * the screen is wide. */

  _scrsize(scr_size);
  bar->width = scr_size[0] - 1;
#endif

  bar->out = config->errors;
}