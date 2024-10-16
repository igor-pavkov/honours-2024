static char *my_get_line(FILE *fp)
{
   char buf[4096];
   char *nl = NULL;
   char *retval = NULL;

   do {
     if (NULL == fgets(buf, sizeof(buf), fp))
       break;
     if (NULL == retval)
       retval = strdup(buf);
     else {
       if (NULL == (retval = realloc(retval,
                                     strlen(retval) + strlen(buf) + 1)))
         break;
       strcat(retval, buf);
     }
   }
   while (NULL == (nl = strchr(retval, '\n')));

   if (NULL != nl)
     *nl = '\0';

   return retval;
}