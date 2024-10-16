static void go_sleep(long ms)
{
#ifdef HAVE_POLL_FINE
  /* portable subsecond "sleep" */
  poll((void *)0, 0, (int)ms);
#else
  /* systems without poll() need other solutions */

#ifdef WIN32
  /* Windows offers a millisecond sleep */
  Sleep(ms);
#elif defined(MSDOS)
  delay(ms);
#else
  /* Other systems must use select() for this */
  struct timeval timeout;

  timeout.tv_sec = ms/1000;
  ms = ms%1000;
  timeout.tv_usec = ms * 1000;

  select(0, NULL,  NULL, NULL, &timeout);
#endif

#endif
}