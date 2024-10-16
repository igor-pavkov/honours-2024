static int isunder(char *file, char *dir)
{
  char *s1 = xabspath(dir, ABS_FILE), *s2 = xabspath(file, 0), *ss = s2;
  int rc = s1 && s2 && strstart(&ss, s1) && (!*ss || *ss=='/' || ss[-1]=='/');

  free(s2);
  free(s1);

  return rc;
}