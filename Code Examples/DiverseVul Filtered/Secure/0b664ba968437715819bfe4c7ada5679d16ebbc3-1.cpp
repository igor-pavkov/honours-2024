static int parsekeyword(unsigned char **pattern, unsigned char *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
#define KEYLEN 10
  char keyword[KEYLEN] = { 0 };
  int found = FALSE;
  int i;
  unsigned char *p = *pattern;
  for(i = 0; !found; i++) {
    char c = *p++;
    if(i >= KEYLEN)
      return SETCHARSET_FAIL;
    switch(state) {
    case CURLFNM_PKW_INIT:
      if(ISALPHA(c) && ISLOWER(c))
        keyword[i] = c;
      else if(c == ':')
        state = CURLFNM_PKW_DDOT;
      else
        return 0;
      break;
    case CURLFNM_PKW_DDOT:
      if(c == ']')
        found = TRUE;
      else
        return SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  if(strcmp(keyword, "digit") == 0)
    charset[CURLFNM_DIGIT] = 1;
  else if(strcmp(keyword, "alnum") == 0)
    charset[CURLFNM_ALNUM] = 1;
  else if(strcmp(keyword, "alpha") == 0)
    charset[CURLFNM_ALPHA] = 1;
  else if(strcmp(keyword, "xdigit") == 0)
    charset[CURLFNM_XDIGIT] = 1;
  else if(strcmp(keyword, "print") == 0)
    charset[CURLFNM_PRINT] = 1;
  else if(strcmp(keyword, "graph") == 0)
    charset[CURLFNM_GRAPH] = 1;
  else if(strcmp(keyword, "space") == 0)
    charset[CURLFNM_SPACE] = 1;
  else if(strcmp(keyword, "blank") == 0)
    charset[CURLFNM_BLANK] = 1;
  else if(strcmp(keyword, "upper") == 0)
    charset[CURLFNM_UPPER] = 1;
  else if(strcmp(keyword, "lower") == 0)
    charset[CURLFNM_LOWER] = 1;
  else
    return SETCHARSET_FAIL;
  return SETCHARSET_OK;
}