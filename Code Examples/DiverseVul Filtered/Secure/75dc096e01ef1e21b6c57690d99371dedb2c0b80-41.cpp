static void multi_freetimeout(void *user, void *entryptr)
{
  (void)user;

  /* the entry was plain malloc()'ed */
  free(entryptr);
}