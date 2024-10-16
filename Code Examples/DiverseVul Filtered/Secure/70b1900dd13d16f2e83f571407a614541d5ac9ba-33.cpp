static ParameterError getparameter(char *flag, /* f or -long-flag */
                                   char *nextarg, /* NULL if unset */
                                   bool *usedarg, /* set to TRUE if the arg
                                                     has been used */
                                   struct Configurable *config)
{
  char letter;
  char subletter=0; /* subletters can only occur on long options */
  int rc; /* generic return code variable */
  const char *parse=NULL;
  unsigned int j;
  time_t now;
  int hit=-1;
  bool longopt=FALSE;
  bool singleopt=FALSE; /* when true means '-o foo' used '-ofoo' */
  ParameterError err;

  /* single-letter,
     long-name,
     boolean whether it takes an additional argument
     */
  static const struct LongShort aliases[]= {
    /* all these ones, starting with "*" or "$" as a short-option have *no*
       short option to mention. */
    {"*", "url",         TRUE},
    {"*a", "random-file", TRUE},
    {"*b", "egd-file",   TRUE},
    {"*c", "connect-timeout", TRUE},
    {"*d", "ciphers",    TRUE},
    {"*e", "disable-epsv", FALSE},
#ifdef USE_ENVIRONMENT
    {"*f", "environment", FALSE},
#endif
    {"*g", "trace",      TRUE},
    {"*h", "trace-ascii", TRUE},
    {"*i", "limit-rate", TRUE},
    {"*j", "compressed",  FALSE}, /* might take an arg someday */
    {"*k", "digest",     FALSE},
    {"*l", "negotiate",  FALSE},
    {"*m", "ntlm",       FALSE},
    {"*n", "basic",      FALSE},
    {"*o", "anyauth",    FALSE},
#ifdef MSDOS
    {"*p", "wdebug",     FALSE},
#endif
    {"*q", "ftp-create-dirs", FALSE},
    {"*r", "create-dirs", FALSE},
    {"*s", "max-redirs",   TRUE},
    {"*t", "proxy-ntlm",   FALSE},
    {"*u", "crlf",        FALSE},
    {"*v", "stderr",      TRUE},
    {"*w", "interface",   TRUE},
    {"*x", "krb4",        TRUE},
    {"*y", "max-filesize", TRUE},
    {"*z", "disable-eprt", FALSE},
    {"$a", "ftp-ssl",    FALSE},
    {"$b", "ftp-pasv",   FALSE},
    {"$c", "socks5",     TRUE},
    {"$c", "socks",      TRUE}, /* this is how the option was documented but
                                   we prefer the --socks5 version for explicit
                                   version */
    {"$d", "tcp-nodelay",FALSE},
    {"$e", "proxy-digest", FALSE},
    {"$f", "proxy-basic", FALSE},
    {"$g", "retry",      TRUE},
    {"$h", "retry-delay", TRUE},
    {"$i", "retry-max-time", TRUE},
    {"$m", "ftp-account", TRUE},
    {"$n", "proxy-anyauth", FALSE},
    {"$o", "trace-time", FALSE},
    {"$p", "ignore-content-length", FALSE},
    {"$q", "ftp-skip-pasv-ip", FALSE},
    {"$r", "ftp-method", TRUE},
    {"$s", "local-port", TRUE},
    {"$t", "socks4",     TRUE},
    {"$u", "ftp-alternative-to-user", TRUE},
    {"$v", "ftp-ssl-reqd", FALSE},
    {"$w", "no-sessionid", FALSE},
    {"$x", "ftp-ssl-control", FALSE},
    {"$y", "ftp-ssl-ccc", FALSE},
    {"$j", "ftp-ssl-ccc-mode", TRUE},
    {"$z", "libcurl",    TRUE},
    {"$#", "raw",        FALSE},

    {"0", "http1.0",     FALSE},
    {"1", "tlsv1",       FALSE},
    {"2", "sslv2",       FALSE},
    {"3", "sslv3",       FALSE},
    {"4", "ipv4",       FALSE},
    {"6", "ipv6",       FALSE},
    {"a", "append",      FALSE},
    {"A", "user-agent",  TRUE},
    {"b", "cookie",      TRUE},
    {"B", "use-ascii",   FALSE},
    {"c", "cookie-jar",  TRUE},
    {"C", "continue-at", TRUE},
    {"d", "data",        TRUE},
    {"da", "data-ascii", TRUE},
    {"db", "data-binary", TRUE},
    {"D", "dump-header", TRUE},
    {"e", "referer",     TRUE},
    {"E", "cert",        TRUE},
    {"Ea", "cacert",     TRUE},
    {"Eb","cert-type",   TRUE},
    {"Ec","key",         TRUE},
    {"Ed","key-type",    TRUE},
    {"Ee","pass",        TRUE},
    {"Ef","engine",      TRUE},
    {"Eg","capath ",     TRUE},
    {"Eh","pubkey",      TRUE},
    {"f", "fail",        FALSE},
    {"F", "form",        TRUE},
    {"Fs","form-string", TRUE},
    {"g", "globoff",     FALSE},
    {"G", "get",         FALSE},
    {"h", "help",        FALSE},
    {"H", "header",      TRUE},
    {"i", "include",     FALSE},
    {"I", "head",        FALSE},
    {"j", "junk-session-cookies", FALSE},
    {"k", "insecure",    FALSE},
    {"K", "config",      TRUE},
    {"l", "list-only",   FALSE},
    {"L", "location",    FALSE},
    {"Lt", "location-trusted", FALSE},
    {"m", "max-time",    TRUE},
    {"M", "manual",      FALSE},
    {"n", "netrc",       FALSE},
    {"no", "netrc-optional", FALSE},
    {"N", "no-buffer",   FALSE},
    {"o", "output",      TRUE},
    {"O", "remote-name", FALSE},
    {"p", "proxytunnel", FALSE},
    {"P", "ftpport",     TRUE}, /* older version */
    {"P", "ftp-port",    TRUE},
    {"q", "disable",     FALSE},
    {"Q", "quote",       TRUE},
    {"r", "range",       TRUE},
    {"R", "remote-time", FALSE},
    {"s", "silent",      FALSE},
    {"S", "show-error",  FALSE},
    {"t", "telnet-options", TRUE},
    {"T", "upload-file", TRUE},
    {"u", "user",        TRUE},
    {"U", "proxy-user",  TRUE},
    {"v", "verbose",     FALSE},
    {"V", "version",     FALSE},
    {"w", "write-out",   TRUE},
    {"x", "proxy",       TRUE},
    {"X", "request",     TRUE},
    {"X", "http-request", TRUE}, /* OBSOLETE VERSION */
    {"Y", "speed-limit",  TRUE},
    {"y", "speed-time", TRUE},
    {"z", "time-cond",   TRUE},
    {"#", "progress-bar",FALSE},
  };

  if(('-' != flag[0]) ||
     (('-' == flag[0]) && ('-' == flag[1]))) {
    /* this should be a long name */
    char *word=('-' == flag[0])?flag+2:flag;
    size_t fnam=strlen(word);
    int numhits=0;
    for(j=0; j< sizeof(aliases)/sizeof(aliases[0]); j++) {
      if(curlx_strnequal(aliases[j].lname, word, fnam)) {
        longopt = TRUE;
        numhits++;
        if(curlx_strequal(aliases[j].lname, word)) {
          parse = aliases[j].letter;
          hit = j;
          numhits = 1; /* a single unique hit */
          break;
        }
        parse = aliases[j].letter;
        hit = j;
      }
    }
    if(numhits>1) {
      /* this is at least the second match! */
      return PARAM_OPTION_AMBIGUOUS;
    }
    if(hit < 0) {
      return PARAM_OPTION_UNKNOWN;
    }
  }
  else {
    flag++; /* prefixed with one dash, pass it */
    hit=-1;
    parse = flag;
  }

  do {
    /* we can loop here if we have multiple single-letters */

    if(!longopt) {
      if(NULL != parse) {
        letter = (char)*parse;
      }
      else {
        letter = '\0';
      }
      subletter='\0';
    }
    else {
      letter = parse[0];
      subletter = parse[1];
    }
    *usedarg = FALSE; /* default is that we don't use the arg */

    if(hit < 0) {
      for(j=0; j< sizeof(aliases)/sizeof(aliases[0]); j++) {
        if(letter == aliases[j].letter[0]) {
          hit = j;
          break;
        }
      }
      if(hit < 0) {
        return PARAM_OPTION_UNKNOWN;
      }
    }
    if(hit < 0) {
      return PARAM_OPTION_UNKNOWN;
    }
    if(!longopt && aliases[hit].extraparam && parse[1]) {
      nextarg=(char *)&parse[1]; /* this is the actual extra parameter */
      singleopt=TRUE;   /* don't loop anymore after this */
    }
    else if(!nextarg && aliases[hit].extraparam) {
      return PARAM_REQUIRES_PARAMETER;
    }
    else if(nextarg && aliases[hit].extraparam)
      *usedarg = TRUE; /* mark it as used */

    switch(letter) {
    case '*': /* options without a short option */
      switch(subletter) {
      case 'a': /* random-file */
        GetStr(&config->random_file, nextarg);
        break;
      case 'b': /* egd-file */
        GetStr(&config->egd_file, nextarg);
        break;
      case 'c': /* connect-timeout */
        if(str2num(&config->connecttimeout, nextarg))
          return PARAM_BAD_NUMERIC;
        break;
      case 'd': /* ciphers */
        GetStr(&config->cipher_list, nextarg);
        break;
      case 'e': /* --disable-epsv */
        config->disable_epsv ^= TRUE;
        break;
#ifdef USE_ENVIRONMENT
      case 'f':
        config->writeenv ^= TRUE;
        break;
#endif
      case 'g': /* --trace */
        GetStr(&config->trace_dump, nextarg);
        if(config->tracetype && (config->tracetype != TRACE_BIN))
          warnf(config, "--trace overrides an earlier trace/verbose option\n");
        config->tracetype = TRACE_BIN;
        break;
      case 'h': /* --trace-ascii */
        GetStr(&config->trace_dump, nextarg);
        if(config->tracetype && (config->tracetype != TRACE_ASCII))
          warnf(config,
                "--trace-ascii overrides an earlier trace/verbose option\n");
        config->tracetype = TRACE_ASCII;
        break;
      case 'i': /* --limit-rate */
        {
          /* We support G, M, K too */
          char *unit;
          curl_off_t value = curlx_strtoofft(nextarg, &unit, 0);

          if(!*unit)
            unit=(char *)"b";
          else if(strlen(unit) > 1)
            unit=(char *)"w"; /* unsupported */

          switch(*unit) {
          case 'G':
          case 'g':
            value *= 1024*1024*1024;
            break;
          case 'M':
          case 'm':
            value *= 1024*1024;
            break;
          case 'K':
          case 'k':
            value *= 1024;
            break;
          case 'b':
          case 'B':
            /* for plain bytes, leave as-is */
            break;
          default:
            warnf(config, "unsupported rate unit. Use G, M, K or B!\n");
            return PARAM_BAD_USE;
          }
          config->recvpersecond = value;
          config->sendpersecond = value;
        }
        break;

      case 'j': /* --compressed */
        config->encoding ^= TRUE;
        break;

      case 'k': /* --digest */
        config->authtype = CURLAUTH_DIGEST;
        break;

      case 'l': /* --negotiate */
        if(curlinfo->features & CURL_VERSION_GSSNEGOTIATE)
          config->authtype = CURLAUTH_GSSNEGOTIATE;
        else
          return PARAM_LIBCURL_DOESNT_SUPPORT;
        break;

      case 'm': /* --ntlm */
        if(curlinfo->features & CURL_VERSION_NTLM)
          config->authtype = CURLAUTH_NTLM;
        else
          return PARAM_LIBCURL_DOESNT_SUPPORT;
        break;

      case 'n': /* --basic for completeness */
        config->authtype = CURLAUTH_BASIC;
        break;

      case 'o': /* --anyauth, let libcurl pick it */
        config->authtype = CURLAUTH_ANY;
        break;

#ifdef MSDOS
      case 'p': /* --wdebug */
        dbug_init();
        break;
#endif
      case 'q': /* --ftp-create-dirs */
        config->ftp_create_dirs ^= TRUE;
        break;

      case 'r': /* --create-dirs */
        config->create_dirs = TRUE;
        break;

      case 's': /* --max-redirs */
        /* specified max no of redirects (http(s)) */
        if(str2num(&config->maxredirs, nextarg))
          return PARAM_BAD_NUMERIC;
        break;

      case 't': /* --proxy-ntlm */
        if(curlinfo->features & CURL_VERSION_NTLM)
          config->proxyntlm ^= TRUE;
        else
          return PARAM_LIBCURL_DOESNT_SUPPORT;
        break;

      case 'u': /* --crlf */
        /* LF -> CRLF conversinon? */
        config->crlf = TRUE;
        break;

      case 'v': /* --stderr */
        if(strcmp(nextarg, "-")) {
          FILE *newfile = fopen(nextarg, "wt");
          if(!newfile)
            warnf(config, "Failed to open %s!\n", nextarg);
          else {
            config->errors = newfile;
            config->errors_fopened = TRUE;
          }
        }
        else
          config->errors = stdout;
      break;
      case 'w': /* --interface */
        /* interface */
        GetStr(&config->iface, nextarg);
        break;
      case 'x': /* --krb4 */
        /* krb4 level string */
        if(curlinfo->features & CURL_VERSION_KERBEROS4)
          GetStr(&config->krb4level, nextarg);
        else
          return PARAM_LIBCURL_DOESNT_SUPPORT;
        break;
      case 'y': /* --max-filesize */
        if(str2offset(&config->max_filesize, nextarg))
          return PARAM_BAD_NUMERIC;
        break;
      case 'z': /* --disable-eprt */
        config->disable_eprt ^= TRUE;
        break;

      default: /* the URL! */
        {
          struct getout *url;
          if(config->url_get || (config->url_get=config->url_list)) {
            /* there's a node here, if it already is filled-in continue to find
               an "empty" node */
            while(config->url_get && (config->url_get->flags&GETOUT_URL))
              config->url_get = config->url_get->next;
          }

          /* now there might or might not be an available node to fill in! */

          if(config->url_get)
            /* existing node */
            url = config->url_get;
          else
            /* there was no free node, create one! */
            url=new_getout(config);

          if(url) {
            /* fill in the URL */
            GetStr(&url->url, nextarg);
            url->flags |= GETOUT_URL;
          }
        }
      }
      break;
    case '$': /* more options without a short option */
      switch(subletter) {
      case 'a': /* --ftp-ssl */
        config->ftp_ssl ^= TRUE;
        break;
      case 'b': /* --ftp-pasv */
        if(config->ftpport)
          free(config->ftpport);
        config->ftpport = NULL;
        break;
      case 'c': /* --socks5 specifies a socks5 proxy to use */
        GetStr(&config->socksproxy, nextarg);
        config->socksver = CURLPROXY_SOCKS5;
        break;
      case 't': /* --socks4 specifies a socks4 proxy to use */
        GetStr(&config->socksproxy, nextarg);
        config->socksver = CURLPROXY_SOCKS4;
        break;
      case 'd': /* --tcp-nodelay option */
        config->tcp_nodelay ^= TRUE;
        break;
      case 'e': /* --proxy-digest */
        config->proxydigest ^= TRUE;
        break;
      case 'f': /* --proxy-basic */
        config->proxybasic ^= TRUE;
        break;
      case 'g': /* --retry */
        if(str2num(&config->req_retry, nextarg))
          return PARAM_BAD_NUMERIC;
        break;
      case 'h': /* --retry-delay */
        if(str2num(&config->retry_delay, nextarg))
          return PARAM_BAD_NUMERIC;
        break;
      case 'i': /* --retry-max-time */
        if(str2num(&config->retry_maxtime, nextarg))
          return PARAM_BAD_NUMERIC;
        break;

      case 'm': /* --ftp-account */
        GetStr(&config->ftp_account, nextarg);
        break;
      case 'n': /* --proxy-anyauth */
        config->proxyanyauth ^= TRUE;
        break;
      case 'o': /* --trace-time */
        config->tracetime ^= TRUE;
        break;
      case 'p': /* --ignore-content-length */
        config->ignorecl ^= TRUE;
        break;
      case 'q': /* --ftp-skip-pasv-ip */
        config->ftp_skip_ip ^= TRUE;
        break;
      case 'r': /* --ftp-method (undocumented at this point) */
        config->ftp_filemethod = ftpfilemethod(config, nextarg);
        break;
      case 's': /* --local-port */
        rc = sscanf(nextarg, "%d - %d",
                    &config->localport,
                    &config->localportrange);
        if(!rc)
          return PARAM_BAD_USE;
        else if(rc == 1)
          config->localportrange = 1; /* default number of ports to try */
        else {
          config->localportrange -= config->localport;
          if(config->localportrange < 1) {
            warnf(config, "bad range input\n");
            return PARAM_BAD_USE;
          }
        }
        break;
      case 'u': /* --ftp-alternative-to-user */
        GetStr(&config->ftp_alternative_to_user, nextarg);
        break;
      case 'v': /* --ftp-ssl-reqd */
        config->ftp_ssl_reqd ^= TRUE;
        break;
      case 'w': /* --no-sessionid */
        config->disable_sessionid ^= TRUE;
        break;
      case 'x': /* --ftp-ssl-control */
        config->ftp_ssl_control ^= TRUE;
        break;
      case 'y': /* --ftp-ssl-ccc */
        config->ftp_ssl_ccc ^= TRUE;
        if(!config->ftp_ssl_ccc_mode)
            config->ftp_ssl_ccc_mode = CURLFTPSSL_CCC_PASSIVE;
        break;
      case 'j': /* --ftp-ssl-ccc-mode */
        config->ftp_ssl_ccc = TRUE;
        config->ftp_ssl_ccc_mode = ftpcccmethod(config, nextarg);
        break;
      case 'z': /* --libcurl */
        GetStr(&config->libcurl, nextarg);
        break;
      case '#': /* --raw */
        config->raw ^= TRUE;
        break;
      }
      break;
    case '#': /* --progress-bar */
      config->progressmode ^= CURL_PROGRESS_BAR;
      break;
    case '0':
      /* HTTP version 1.0 */
      config->httpversion = CURL_HTTP_VERSION_1_0;
      break;
    case '1':
      /* TLS version 1 */
      config->ssl_version = CURL_SSLVERSION_TLSv1;
      break;
    case '2':
      /* SSL version 2 */
      config->ssl_version = CURL_SSLVERSION_SSLv2;
      break;
    case '3':
      /* SSL version 3 */
      config->ssl_version = CURL_SSLVERSION_SSLv3;
      break;
    case '4':
      /* IPv4 */
      config->ip_version = 4;
      break;
    case '6':
      /* IPv6 */
      config->ip_version = 6;
      break;
    case 'a':
      /* This makes the FTP sessions use APPE instead of STOR */
      config->conf ^= CONF_FTPAPPEND;
      break;
    case 'A':
      /* This specifies the User-Agent name */
      GetStr(&config->useragent, nextarg);
      break;
    case 'b': /* cookie string coming up: */
      if(nextarg[0] == '@') {
        nextarg++;
      }
      else if(strchr(nextarg, '=')) {
        /* A cookie string must have a =-letter */
        GetStr(&config->cookie, nextarg);
        break;
      }
      /* We have a cookie file to read from! */
      GetStr(&config->cookiefile, nextarg);
      break;
    case 'B':
      /* use ASCII/text when transfering */
      config->conf ^= CONF_GETTEXT;
      break;
    case 'c':
      /* get the file name to dump all cookies in */
      GetStr(&config->cookiejar, nextarg);
      break;
    case 'C':
      /* This makes us continue an ftp transfer at given position */
      if(!curlx_strequal(nextarg, "-")) {
        if(str2offset(&config->resume_from, nextarg))
          return PARAM_BAD_NUMERIC;
        config->resume_from_current = FALSE;
      }
      else {
        config->resume_from_current = TRUE;
        config->resume_from = 0;
      }
      config->use_resume=TRUE;
      break;
    case 'd':
      /* postfield data */
      {
        char *postdata=NULL;

        if('@' == *nextarg) {
          /* the data begins with a '@' letter, it means that a file name
             or - (stdin) follows */
          FILE *file;

          nextarg++; /* pass the @ */

          if(curlx_strequal("-", nextarg)) {
            file = stdin;
            if(subletter == 'b') /* forced binary */
              SET_BINMODE(stdin);
          }
          else {
            file = fopen(nextarg, "rb");
            if(!file)
              warnf(config, "Couldn't read data from file \"%s\", this makes "
                    "an empty POST.\n", nextarg);
          }

          if(subletter == 'b') /* forced binary */
            postdata = file2memory(file, &config->postfieldsize);
          else
            postdata = file2string(file);

          if(file && (file != stdin))
            fclose(file);

          if(!postdata) {
            /* no data from the file, point to a zero byte string to make this
               get sent as a POST anyway */
            postdata=strdup("");
          }
        }
        else {
          GetStr(&postdata, nextarg);
        }

#ifdef CURL_DOES_CONVERSIONS
        if(subletter != 'b') { /* NOT forced binary, convert to ASCII */
          convert_to_network(postdata, strlen(postdata));
        }
#endif

        if(config->postfields) {
          /* we already have a string, we append this one
             with a separating &-letter */
          char *oldpost=config->postfields;
          size_t newlen = strlen(oldpost) + strlen(postdata) + 2;
          config->postfields=malloc(newlen);
          if(!config->postfields) {
            free(postdata);
            return PARAM_NO_MEM;
          }
          /* use ASCII value 0x26 for '&' to accommodate non-ASCII platforms */
          snprintf(config->postfields, newlen, "%s\x26%s", oldpost, postdata);
          free(oldpost);
          free(postdata);
        }
        else
          config->postfields=postdata;
      }
      /*
        We can't set the request type here, as this data might be used in
        a simple GET if -G is used. Already or soon.

        if(SetHTTPrequest(HTTPREQ_SIMPLEPOST, &config->httpreq))
          return PARAM_BAD_USE;
      */
      break;
    case 'D':
      /* dump-header to given file name */
      GetStr(&config->headerfile, nextarg);
      break;
    case 'e':
      {
        char *ptr = strstr(nextarg, ";auto");
        if(ptr) {
          /* Automatic referer requested, this may be combined with a
             set initial one */
          config->conf |= CONF_AUTO_REFERER;
          *ptr = 0; /* zero terminate here */
        }
        GetStr(&config->referer, nextarg);
      }
      break;
    case 'E':
      switch(subletter) {
      case 'a': /* CA info PEM file */
        /* CA info PEM file */
        GetStr(&config->cacert, nextarg);
        break;
      case 'b': /* cert file type */
        GetStr(&config->cert_type, nextarg);
        break;
      case 'c': /* private key file */
        GetStr(&config->key, nextarg);
        break;
      case 'd': /* private key file type */
        GetStr(&config->key_type, nextarg);
        break;
      case 'e': /* private key passphrase */
        GetStr(&config->key_passwd, nextarg);
        cleanarg(nextarg);
        break;
      case 'f': /* crypto engine */
        GetStr(&config->engine, nextarg);
        if (config->engine && curlx_strequal(config->engine,"list"))
           config->list_engines = TRUE;
        break;
      case 'g': /* CA info PEM file */
        /* CA cert directory */
        GetStr(&config->capath, nextarg);
        break;
      case 'h': /* --pubkey public key file */
        GetStr(&config->pubkey, nextarg);
        break;
      default: /* certificate file */
        {
          char *ptr = strchr(nextarg, ':');
          /* Since we live in a world of weirdness and confusion, the win32
             dudes can use : when using drive letters and thus
             c:\file:password needs to work. In order not to break
             compatibility, we still use : as separator, but we try to detect
             when it is used for a file name! On windows. */
#ifdef WIN32
          if(ptr &&
             (ptr == &nextarg[1]) &&
             (nextarg[2] == '\\' || nextarg[2] == '/') &&
             (ISALPHA(nextarg[0])) )
             /* colon in the second column, followed by a backslash, and the
                first character is an alphabetic letter:

                this is a drive letter colon */
            ptr = strchr(&nextarg[3], ':'); /* find the next one instead */
#endif
          if(ptr) {
            /* we have a password too */
            *ptr=0;
            ptr++;
            GetStr(&config->key_passwd, ptr);
          }
          GetStr(&config->cert, nextarg);
          cleanarg(nextarg);
        }
      }
      break;
    case 'f':
      /* fail hard on errors  */
      config->conf ^= CONF_FAILONERROR;
      break;
    case 'F':
      /* "form data" simulation, this is a little advanced so lets do our best
         to sort this out slowly and carefully */
      if(formparse(config,
                   nextarg,
                   &config->httppost,
                   &config->last_post,
                   (bool) (subletter=='s'))) /* 's' means literal string */
        return PARAM_BAD_USE;
      if(SetHTTPrequest(config, HTTPREQ_POST, &config->httpreq))
        return PARAM_BAD_USE;
      break;

    case 'g': /* g disables URLglobbing */
      config->globoff ^= TRUE;
      break;

    case 'G': /* HTTP GET */
      config->use_httpget = TRUE;
      break;

    case 'h': /* h for help */
      help();
      return PARAM_HELP_REQUESTED;
    case 'H':
      /* A custom header to append to a list */
      err = add2list(&config->headers, nextarg);
      if(err)
        return err;
      break;
    case 'i':
      config->conf ^= CONF_HEADER; /* include the HTTP header as well */
      break;
    case 'j':
      config->cookiesession ^= TRUE;
      break;
    case 'I':
      /*
       * This is a bit tricky. We either SET both bits, or we clear both
       * bits. Let's not make any other outcomes from this.
       */
      if((CONF_HEADER|CONF_NOBODY) !=
         (config->conf&(CONF_HEADER|CONF_NOBODY)) ) {
        /* one of them weren't set, set both */
        config->conf |= (CONF_HEADER|CONF_NOBODY);
        if(SetHTTPrequest(config, HTTPREQ_HEAD, &config->httpreq))
          return PARAM_BAD_USE;
      }
      else {
        /* both were set, clear both */
        config->conf &= ~(CONF_HEADER|CONF_NOBODY);
        if(SetHTTPrequest(config, HTTPREQ_GET, &config->httpreq))
          return PARAM_BAD_USE;
      }
      break;
    case 'k': /* allow insecure SSL connects */
      config->insecure_ok ^= TRUE;
      break;
    case 'K': /* parse config file */
      if(parseconfig(nextarg, config))
        warnf(config, "error trying read config from the '%s' file\n",
              nextarg);
      break;
    case 'l':
      config->conf ^= CONF_FTPLISTONLY; /* only list the names of the FTP dir */
      break;
    case 'L':
      config->conf ^= CONF_FOLLOWLOCATION; /* Follow Location: HTTP headers */
      switch (subletter) {
      case 't':
        /* Continue to send authentication (user+password) when following
         * locations, even when hostname changed */
        config->conf ^= CONF_UNRESTRICTED_AUTH;
        break;
      }
      break;
    case 'm':
      /* specified max time */
      if(str2num(&config->timeout, nextarg))
        return PARAM_BAD_NUMERIC;
      break;
    case 'M': /* M for manual, huge help */
#ifdef USE_MANUAL
      hugehelp();
      return PARAM_HELP_REQUESTED;
#else
      warnf(config,
            "built-in manual was disabled at build-time!\n");
      return PARAM_OPTION_UNKNOWN;
#endif
    case 'n':
      switch(subletter) {
      case 'o': /* CA info PEM file */
        /* use .netrc or URL */
        config->conf ^= CONF_NETRC_OPT;
        break;
      default:
        /* pick info from .netrc, if this is used for http, curl will
           automatically enfore user+password with the request */
        config->conf ^= CONF_NETRC;
        break;
      }
      break;
    case 'N':
      /* disable the output I/O buffering */
      config->nobuffer ^= 1;
      break;
    case 'o':
    case 'O':
      /* output file */
      {
        struct getout *url;
        if(config->url_out || (config->url_out=config->url_list)) {
          /* there's a node here, if it already is filled-in continue to find
             an "empty" node */
          while(config->url_out && (config->url_out->flags&GETOUT_OUTFILE))
            config->url_out = config->url_out->next;
        }

        /* now there might or might not be an available node to fill in! */

        if(config->url_out)
          /* existing node */
          url = config->url_out;
        else
          /* there was no free node, create one! */
          url=new_getout(config);

        if(url) {
          /* fill in the outfile */
          if('o' == letter)
            GetStr(&url->outfile, nextarg);
          else {
            url->outfile=NULL; /* leave it */
            url->flags |= GETOUT_USEREMOTE;
          }
          url->flags |= GETOUT_OUTFILE;
        }
      }
      break;
    case 'P':
      /* This makes the FTP sessions use PORT instead of PASV */
      /* use <eth0> or <192.168.10.10> style addresses. Anything except
         this will make us try to get the "default" address.
         NOTE: this is a changed behaviour since the released 4.1!
         */
      GetStr(&config->ftpport, nextarg);
      break;
    case 'p':
      /* proxy tunnel for non-http protocols */
      config->proxytunnel ^= TRUE;
      break;

    case 'q': /* if used first, already taken care of, we do it like
                 this so we don't cause an error! */
      break;
    case 'Q':
      /* QUOTE command to send to FTP server */
      err = PARAM_OK;
      switch(nextarg[0]) {
      case '-':
        /* prefixed with a dash makes it a POST TRANSFER one */
        nextarg++;
        err = add2list(&config->postquote, nextarg);
        break;
      case '+':
        /* prefixed with a plus makes it a just-before-transfer one */
        nextarg++;
        err = add2list(&config->prequote, nextarg);
        break;
      default:
        err = add2list(&config->quote, nextarg);
        break;
      }
      if(err)
        return err;
      break;
    case 'r':
      /* Specifying a range WITHOUT A DASH will create an illegal HTTP range
         (and won't actually be range by definition). The man page previously
         claimed that to be a good way, why this code is added to work-around
         it. */
      if(ISDIGIT(*nextarg) && !strchr(nextarg, '-')) {
        char buffer[32];
        curl_off_t off;
        warnf(config,
              "A specfied range MUST include at least one dash (-). "
              "Appending one for you!\n");
        off = curlx_strtoofft(nextarg, NULL, 10);
        snprintf(buffer, sizeof(buffer), "%Od-", off);
        GetStr(&config->range, buffer);
      }
      {
        /* byte range requested */
        char* tmp_range;
        tmp_range=nextarg;
        while(*tmp_range != '\0') {
          if(!ISDIGIT(*tmp_range)&&*tmp_range!='-'&&*tmp_range!=',') {
            warnf(config,"Invalid character is found in given range. "
                  "A specified range MUST have only digits in "
                  "\'start\'-\'stop\'. The server's response to this "
                  "request is uncertain.\n");
            break;
          }
          tmp_range++;
        }
        /* byte range requested */
        GetStr(&config->range, nextarg);
      }
      break;
    case 'R':
      /* use remote file's time */
      config->remote_time ^= TRUE;
      break;
    case 's':
      /* don't show progress meter, don't show errors : */
      config->conf |= (CONF_MUTE|CONF_NOPROGRESS);
      config->showerror ^= TRUE; /* toggle off */
      break;
    case 'S':
      /* show errors */
      config->showerror ^= TRUE; /* toggle on if used with -s */
      break;
    case 't':
      /* Telnet options */
      err = add2list(&config->telnet_options, nextarg);
      if(err)
        return err;
      break;
    case 'T':
      /* we are uploading */
      {
        struct getout *url;
        if(config->url_out || (config->url_out=config->url_list)) {
          /* there's a node here, if it already is filled-in continue to find
             an "empty" node */
          while(config->url_out && (config->url_out->flags&GETOUT_UPLOAD))
            config->url_out = config->url_out->next;
        }

        /* now there might or might not be an available node to fill in! */

        if(config->url_out)
          /* existing node */
          url = config->url_out;
        else
          /* there was no free node, create one! */
          url=new_getout(config);

        if(url) {
          url->flags |= GETOUT_UPLOAD; /* mark -T used */
          if(!*nextarg)
            url->flags |= GETOUT_NOUPLOAD;
          else {
            /* "-" equals stdin, but keep the string around for now */
            GetStr(&url->infile, nextarg);
          }
        }
      }
      break;
    case 'u':
      /* user:password  */
      GetStr(&config->userpwd, nextarg);
      cleanarg(nextarg);
      checkpasswd("host", &config->userpwd);
      break;
    case 'U':
      /* Proxy user:password  */
      GetStr(&config->proxyuserpwd, nextarg);
      cleanarg(nextarg);
      checkpasswd("proxy", &config->proxyuserpwd);
      break;
    case 'v':
      /* the '%' thing here will cause the trace get sent to stderr */
      GetStr(&config->trace_dump, (char *)"%");
      if(config->tracetype && (config->tracetype != TRACE_PLAIN))
        warnf(config,
              "-v/--verbose overrides an earlier trace/verbose option\n");
      config->tracetype = TRACE_PLAIN;
      break;
    case 'V':
    {
      const char * const *proto;

      printf(CURL_ID "%s\n", curl_version());
      if (curlinfo->protocols) {
        printf("Protocols: ");
        for (proto=curlinfo->protocols; *proto; ++proto) {
          printf("%s ", *proto);
        }
        puts(""); /* newline */
      }
      if(curlinfo->features) {
        unsigned int i;
        struct feat {
          const char *name;
          int bitmask;
        };
        static const struct feat feats[] = {
          {"AsynchDNS", CURL_VERSION_ASYNCHDNS},
          {"Debug", CURL_VERSION_DEBUG},
          {"GSS-Negotiate", CURL_VERSION_GSSNEGOTIATE},
          {"IDN", CURL_VERSION_IDN},
          {"IPv6", CURL_VERSION_IPV6},
          {"Largefile", CURL_VERSION_LARGEFILE},
          {"NTLM", CURL_VERSION_NTLM},
          {"SPNEGO", CURL_VERSION_SPNEGO},
          {"SSL",  CURL_VERSION_SSL},
          {"SSPI",  CURL_VERSION_SSPI},
          {"krb4", CURL_VERSION_KERBEROS4},
          {"libz", CURL_VERSION_LIBZ},
          {"CharConv", CURL_VERSION_CONV}
        };
        printf("Features: ");
        for(i=0; i<sizeof(feats)/sizeof(feats[0]); i++) {
          if(curlinfo->features & feats[i].bitmask)
            printf("%s ", feats[i].name);
        }
        puts(""); /* newline */
      }
    }
    return PARAM_HELP_REQUESTED;
    case 'w':
      /* get the output string */
      if('@' == *nextarg) {
        /* the data begins with a '@' letter, it means that a file name
           or - (stdin) follows */
        FILE *file;
        nextarg++; /* pass the @ */
        if(curlx_strequal("-", nextarg))
          file = stdin;
        else
          file = fopen(nextarg, "r");
        config->writeout = file2string(file);
        if(!config->writeout)
          warnf(config, "Failed to read %s", file);
        if(file && (file != stdin))
          fclose(file);
      }
      else
        GetStr(&config->writeout, nextarg);
      break;
    case 'x':
      /* proxy */
      GetStr(&config->proxy, nextarg);
      break;
    case 'X':
      /* set custom request */
      GetStr(&config->customrequest, nextarg);
      break;
    case 'y':
      /* low speed time */
      if(str2num(&config->low_speed_time, nextarg))
        return PARAM_BAD_NUMERIC;
      if(!config->low_speed_limit)
        config->low_speed_limit = 1;
      break;
    case 'Y':
      /* low speed limit */
      if(str2num(&config->low_speed_limit, nextarg))
        return PARAM_BAD_NUMERIC;
      if(!config->low_speed_time)
        config->low_speed_time=30;
      break;
    case 'z': /* time condition coming up */
      switch(*nextarg) {
      case '+':
        nextarg++;
      default:
        /* If-Modified-Since: (section 14.28 in RFC2068) */
        config->timecond = CURL_TIMECOND_IFMODSINCE;
        break;
      case '-':
        /* If-Unmodified-Since:  (section 14.24 in RFC2068) */
        config->timecond = CURL_TIMECOND_IFUNMODSINCE;
        nextarg++;
        break;
      case '=':
        /* Last-Modified:  (section 14.29 in RFC2068) */
        config->timecond = CURL_TIMECOND_LASTMOD;
        nextarg++;
        break;
      }
      now=time(NULL);
      config->condtime=curl_getdate(nextarg, &now);
      if(-1 == (int)config->condtime) {
        /* now let's see if it is a file name to get the time from instead! */
        struct_stat statbuf;
        if(-1 == stat(nextarg, &statbuf)) {
          /* failed, remove time condition */
          config->timecond = CURL_TIMECOND_NONE;
          warnf(config,
                "Illegal date format for -z/--timecond (and not "
                "a file name). Disabling time condition. "
                "See curl_getdate(3) for valid date syntax.\n");
        }
        else {
          /* pull the time out from the file */
          config->condtime = statbuf.st_mtime;
        }
      }
      break;
    default: /* unknown flag */
      return PARAM_OPTION_UNKNOWN;
    }
    hit = -1;

  } while(!longopt && !singleopt && *++parse && !*usedarg);

  return PARAM_OK;
}