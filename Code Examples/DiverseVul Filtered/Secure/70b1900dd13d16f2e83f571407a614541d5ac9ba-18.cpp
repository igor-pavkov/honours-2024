static void GetStr(char **string,
                   char *value)
{
  if(*string)
    free(*string);
  if(value)
    *string = strdup(value);
  else
    *string = NULL;
}