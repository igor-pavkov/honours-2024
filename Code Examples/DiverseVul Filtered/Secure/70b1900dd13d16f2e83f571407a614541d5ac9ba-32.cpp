static void cleanarg(char *str)
{
#ifdef HAVE_WRITABLE_ARGV
  /* now that GetStr has copied the contents of nextarg, wipe the next
   * argument out so that the username:password isn't displayed in the
   * system process list */
  if (str) {
    size_t len = strlen(str);
    memset(str, ' ', len);
  }
#else
  (void)str;
#endif
}