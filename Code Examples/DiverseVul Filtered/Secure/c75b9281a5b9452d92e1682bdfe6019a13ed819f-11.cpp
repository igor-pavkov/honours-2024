static char *dupstr(const char *src)
{
  char *dst = malloc(strlen(src) + 1);
  if (!dst) {
    fprintf(stderr, "%s:%d: malloc failed!\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }
  char *cptr = dst;
  while ((*cptr++ = *src++))
    ;
  return dst;
}