static void strstore(char **str, const char *newstr)
{
  free(*str);
  *str = strdup(newstr);
}