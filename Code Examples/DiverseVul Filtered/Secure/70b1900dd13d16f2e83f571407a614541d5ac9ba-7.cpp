static int parseconfig(const char *filename,
                       struct Configurable *config)
{
  int res;
  FILE *file;
  char filebuffer[512];
  bool usedarg;
  char *home;

  if(!filename || !*filename) {
    /* NULL or no file name attempts to load .curlrc from the homedir! */

#define CURLRC DOT_CHAR "curlrc"

#ifndef __AMIGA__
    filename = CURLRC;   /* sensible default */
    home = homedir();    /* portable homedir finder */
    if(home) {
      if(strlen(home)<(sizeof(filebuffer)-strlen(CURLRC))) {
        snprintf(filebuffer, sizeof(filebuffer),
                 "%s%s%s", home, DIR_CHAR, CURLRC);

#ifdef WIN32
        /* Check if the file exists - if not, try CURLRC in the same
         * directory as our executable
         */
        file = fopen(filebuffer, "r");
        if (file != NULL) {
          fclose(file);
          filename = filebuffer;
        }
        else {
          /* Get the filename of our executable. GetModuleFileName is
           * already declared via inclusions done in setup header file.
           * We assume that we are using the ASCII version here.
           */
          int n = GetModuleFileName(0, filebuffer, sizeof(filebuffer));
          if (n > 0 && n < (int)sizeof(filebuffer)) {
            /* We got a valid filename - get the directory part */
            char *lastdirchar = strrchr(filebuffer, '\\');
            if (lastdirchar) {
              int remaining;
              *lastdirchar = 0;
              /* If we have enough space, build the RC filename */
              remaining = sizeof(filebuffer) - strlen(filebuffer);
              if ((int)strlen(CURLRC) < remaining - 1) {
                snprintf(lastdirchar, remaining,
                         "%s%s", DIR_CHAR, CURLRC);
                /* Don't bother checking if it exists - we do
                 * that later
                 */
                filename = filebuffer;
              }
            }
          }
        }
#else /* WIN32 */
        filename = filebuffer;
#endif /* WIN32 */
      }
      free(home); /* we've used it, now free it */
    }

# else /* __AMIGA__ */
  /* On AmigaOS all the config files are into env:
   */
  filename = "ENV:" CURLRC;

#endif
  }

  if(strcmp(filename,"-"))
    file = fopen(filename, "r");
  else
    file = stdin;

  if(file) {
    char *line;
    char *aline;
    char *option;
    char *param;
    int lineno=0;
    bool alloced_param;

#define ISSEP(x) (((x)=='=') || ((x) == ':'))

    while (NULL != (aline = my_get_line(file))) {
      lineno++;
      line = aline;
      alloced_param=FALSE;

      /* lines with # in the fist column is a comment! */
      while(*line && ISSPACE(*line))
        line++;

      switch(*line) {
      case '#':
      case '/':
      case '\r':
      case '\n':
      case '*':
      case '\0':
        free(aline);
        continue;
      }

      /* the option keywords starts here */
      option = line;
      while(*line && !ISSPACE(*line) && !ISSEP(*line))
        line++;
      /* ... and has ended here */

      if(*line)
        *line++=0; /* zero terminate, we have a local copy of the data */

#ifdef DEBUG_CONFIG
      fprintf(stderr, "GOT: %s\n", option);
#endif

      /* pass spaces and separator(s) */
      while(*line && (ISSPACE(*line) || ISSEP(*line)))
        line++;

      /* the parameter starts here (unless quoted) */
      if(*line == '\"') {
        char *ptr;
        /* quoted parameter, do the qoute dance */
        line++;
        param=strdup(line); /* parameter */
        alloced_param=TRUE;

        ptr=param;
        while(*line && (*line != '\"')) {
          if(*line == '\\') {
            char out;
            line++;

            /* default is to output the letter after the backslah */
            switch(out = *line) {
            case '\0':
              continue; /* this'll break out of the loop */
            case 't':
              out='\t';
              break;
            case 'n':
              out='\n';
              break;
            case 'r':
              out='\r';
              break;
            case 'v':
              out='\v';
              break;
            }
            *ptr++=out;
            line++;
          }
          else
            *ptr++=*line++;
        }
        *ptr=0; /* always zero terminate */

      }
      else {
        param=line; /* parameter starts here */
        while(*line && !ISSPACE(*line))
          line++;
        *line=0; /* zero terminate */
      }

      if (param && !*param) {
        /* do this so getparameter can check for required parameters.
           Otherwise it always thinks there's a parameter. */
        if (alloced_param)
          free(param);
        param = NULL;
      }

#ifdef DEBUG_CONFIG
      fprintf(stderr, "PARAM: \"%s\"\n",(param ? param : "(null)"));
#endif
      res = getparameter(option, param, &usedarg, config);

      if (param && *param && !usedarg)
        /* we passed in a parameter that wasn't used! */
        res = PARAM_GOT_EXTRA_PARAMETER;

      if(res != PARAM_OK) {
        /* the help request isn't really an error */
        if(!strcmp(filename, "-")) {
          filename=(char *)"<stdin>";
        }
        if(PARAM_HELP_REQUESTED != res) {
          const char *reason = param2text(res);
          warnf(config, "%s:%d: warning: '%s' %s\n",
                filename, lineno, option, reason);
        }
      }

      if(alloced_param)
      {
        free(param);
        param = NULL;
      }

      free(aline);
    }
    if(file != stdin)
      fclose(file);
  }
  else
    return 1; /* couldn't open the file */
  return 0;
}