const char *am_get_mime_header(request_rec *r, const char *m, const char *h) 
{
    const char *line;
    char *l1;
    const char *value;
    char *l2;

    for (line = am_xstrtok(r, m, "\n", &l1); line && *line; 
         line = am_xstrtok(r, NULL, "\n", &l1)) {

        am_strip_blank(&line);

        if (((value = am_xstrtok(r, line, ":", &l2)) != NULL) &&
            (strcasecmp(value, h) == 0)) {
            if ((value = am_xstrtok(r, NULL, ":", &l2)) != NULL)
                am_strip_blank(&value);
            return value;
        }
   }
   return NULL;
}