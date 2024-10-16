static void dumpeasycode(struct Configurable *config)
{
  struct curl_slist *ptr = easycode;
  char *o = config->libcurl;

  if(o) {
    FILE *out;
    bool fopened = FALSE;
    if(strcmp(o, "-")) {
      out = fopen(o, "wt");
      fopened = TRUE;
    }
    else
      out= stdout;
    if(!out)
      warnf(config, "Failed to open %s to write libcurl code!\n", o);
    else {
      int i;
      const char *c;

      for(i=0; (c = srchead[i]); i++) {
        if(!memcmp((char *)c, "[m]", 3)) {
#if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS > 32)
          fprintf(out, "#define _FILE_OFFSET_BITS %d "
                  "/* for curl_off_t magic */\n",
                  _FILE_OFFSET_BITS);
#endif
        }
        else
          fprintf(out, "%s\n", c);
      }

      while(ptr) {
        fprintf(out, "  %s\n", ptr->data);
        ptr = ptr->next;
      }
      fprintf(out,
              "}\n"
              "/* */\n");
      if(fopened)
        fclose(out);
    }
  }
  curl_slist_free_all(easycode);
}