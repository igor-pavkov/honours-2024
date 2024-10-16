size_t filter_result_len(const char *filter, const char *user, char *output) {
  const char *part = NULL;
  size_t result = 0;
  do
    {
      size_t len;
      part = strstr(filter, "%u");
      if(part)
        len = part - filter;
      else
        len = strlen(filter);
      if (output)
        {
          strncpy(output, filter, len);
          output += len;
        }
      result += len;
      filter += len + 2;
      if(part)
        {
          if(output)
            {
              strncpy(output, user, strlen(user));
              output += strlen(user);
            }
          result += strlen(user);
        }
    }
  while(part);

  if(output)
    *output = '\0';
  return(result + 1);
}