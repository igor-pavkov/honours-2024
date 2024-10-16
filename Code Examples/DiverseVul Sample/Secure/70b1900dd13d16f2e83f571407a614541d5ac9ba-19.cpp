static char *file2memory(FILE *file, long *size)
{
  char buffer[1024];
  char *string=NULL;
  char *newstring=NULL;
  size_t len=0;
  long stringlen=0;

  if(file) {
    while((len = fread(buffer, 1, sizeof(buffer), file))) {
      if(string) {
        newstring = realloc(string, len+stringlen+1);
        if(newstring)
          string = newstring;
        else
          break; /* no more strings attached! :-) */
      }
      else
        string = malloc(len+1);
      memcpy(&string[stringlen], buffer, len);
      stringlen+=len;
    }
    if (string) {
      /* NUL terminate the buffer in case it's treated as a string later */
      string[stringlen] = 0;
    }
    *size = stringlen;
    return string;
  }
  else
    return NULL; /* no string */
}