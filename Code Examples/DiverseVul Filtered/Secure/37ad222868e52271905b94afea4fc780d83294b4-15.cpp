int pureftpd_start(int argc, char *argv[], const char *home_directory_)
{
#ifndef NO_GETOPT_LONG
    int option_index = 0;
#endif
    int fodder;
    int bypass_ipv6 = 0;
    struct passwd *pw;

    (void) home_directory_;
#ifdef NON_ROOT_FTP
    home_directory = home_directory_;
#endif
    client_init_reply_buf();

#ifdef HAVE_GETPAGESIZE
    page_size = (size_t) getpagesize();
#elif defined(_SC_PAGESIZE)
    page_size = (size_t) sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
    page_size = (size_t) sysconf(_SC_PAGE_SIZE);
#else
    page_size = (size_t) 4096U;
#endif

#ifdef HAVE_SETLOCALE
# ifdef LC_MESSAGES
    (void) setlocale(LC_MESSAGES, MESSAGES_LOCALE);
# endif
# ifdef LC_CTYPE
    (void) setlocale(LC_CTYPE, "C");
# endif
# ifdef LC_COLLATE
    (void) setlocale(LC_COLLATE, "C");
# endif
#endif

    init_tz();
    (void) strerror(ENOENT);

#ifndef SAVE_DESCRIPTORS
    openlog("pure-ftpd", LOG_NDELAY | log_pid, DEFAULT_FACILITY);
#endif

#ifdef USE_CAPABILITIES
    set_initial_caps();
#endif
    set_signals();

    loggedin = 0;

#ifdef BANNER_ENVIRON
# ifdef COOKIE
    {
        const char *a;

        if ((a = getenv("BANNER")) != NULL && *a != 0) {
            fortunes_file = strdup(a);
        }
    }
# endif
#endif

#ifndef MINIMAL
    if (argc == 2 && *argv[1] != '-' &&
        sc_build_command_line_from_file(argv[1], NULL, simpleconf_options,
                                        (sizeof simpleconf_options) /
                                        (sizeof simpleconf_options[0]),
                                        argv[0], &argc, &argv) != 0) {
        die(421, LOG_ERR, MSG_CONF_ERR);
    }
#endif

    while ((fodder =
#ifndef NO_GETOPT_LONG
            getopt_long(argc, argv, GETOPT_OPTIONS, long_options, &option_index)
#else
            getopt(argc, argv, GETOPT_OPTIONS)
#endif
            ) != -1) {
        switch (fodder) {
        case 's': {
            if ((pw = getpwnam("ftp")) != NULL ||
                (pw = getpwnam("_ftp")) != NULL) {
                warez = pw->pw_uid;
            } else {
                logfile(LOG_ERR, MSG_NO_FTP_ACCOUNT);
            }
            break;
        }
        case '0': {
            no_truncate = 1;
            break;
        }
        case '4': {
            bypass_ipv6 = 1;
            break;
        }
        case '6': {
            no_ipv4 = 1;
            break;
        }
        case '1': {
            log_pid = LOG_PID;
            break;
        }
#ifndef NO_STANDALONE
        case 'S': {
            char *struck;

            if ((struck = strchr(optarg, ',')) != NULL) {
                *struck = 0;
                if (*optarg != 0) {
                    if (standalone_ip == NULL &&
                        (standalone_ip = strdup(optarg)) == NULL) {
                        die_mem();
                    }
                }
                *struck = ',';
                if (struck[1] != 0) {
                    if ((standalone_port = strdup(struck + 1)) == NULL) {
                        die_mem();
                    }
                }
            } else {
                if ((standalone_port = strdup(optarg)) == NULL) {
                    die_mem();
                }
            }
            break;
        }
#endif
        case 'D': {
            force_ls_a = 1;
            break;
        }
#ifdef THROTTLING
        case 't':
        case 'T': {
            char *struck;
            const char *tr_bw_ul = NULL;
            const char *tr_bw_dl = NULL;

            if ((struck = strchr(optarg, ':')) != NULL) {
                *struck = 0;
                if (*optarg != 0) {
                    tr_bw_ul = optarg;
                }
                if (struck[1] != 0) {
                    tr_bw_dl = &struck[1];
                }
            } else {
                tr_bw_ul = tr_bw_dl = optarg;
            }
            if ((tr_bw_ul == NULL || *tr_bw_ul == 0) &&
                (tr_bw_dl == NULL || *tr_bw_dl == 0)) {
                bad_bw:
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_THROTTLING ": %s" , optarg);
            }
            if (tr_bw_dl != NULL) {
                if ((throttling_bandwidth_dl =
                     strtoul(tr_bw_dl, NULL, 0) * 1024UL) == 0UL) {
                    goto bad_bw;
                }
            }
            if (tr_bw_ul != NULL) {
                if ((throttling_bandwidth_ul =
                     strtoul(tr_bw_ul, NULL, 0) * 1024UL) == 0UL) {
                    goto bad_bw;
                }
            }
            throttling_delay = 1000000 /
                (throttling_bandwidth_dl | throttling_bandwidth_ul);
            if (fodder == 't') {
                throttling = 1;
            } else {
                throttling = 2;
            }
            break;
        }
#endif
        case 'a': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            chroot_trustedgid = strtoul(nptr, &endptr, 0);
            if (!nptr || !*nptr || !endptr || *endptr) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_TRUSTED_GID ": %s" , optarg);
            }
            userchroot = 1;
            break;
        }
        case 'x': {
            dot_write_ok = 0;
            break;
        }
        case 'X': {
            dot_write_ok = dot_read_ok = 0;
            break;
        }
        case 'z': {
            dot_read_anon_ok = 1;
            break;
        }
        case 'Z': {
            be_customer_proof = 1;
            break;
        }
        case 'A': {
            userchroot = 2;
            break;
        }
        case 'w': {
            allowfxp = 1;
            break;
        }
        case 'W': {
            allowfxp = 2;
            break;
        }
        case 'd': {
            if (logging < 2) {
                logging++;
            }
            break;
        }
        case 'b': {
            broken_client_compat = 1;
            break;
        }
        case 'c': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            maxusers = (unsigned int) strtoul(nptr, &endptr, 0);
            if (!nptr || !*nptr || !endptr || *endptr || !maxusers) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_USER_LIMIT ": %s" , optarg);
            }
            break;
        }
#ifndef NO_STANDALONE
        case 'B': {
            daemonize = 1;
            break;
        }
        case 'C': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            maxip = (unsigned int) strtoul(nptr, &endptr, 0);
            if (!nptr || !*nptr || !endptr || *endptr || !maxip) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_USER_LIMIT ": %s" , optarg);
            }
            break;
        }
#endif
#ifdef PER_USER_LIMITS
        case 'y': {
            int ret;

            ret = sscanf(optarg, "%u:%u", &per_user_max, &per_anon_max);
            if (ret != 2) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_USER_LIMIT ": %s" , optarg);
            }
            break;
        }
#endif
#ifdef WITH_TLS
        case '2': {
            char *struck;
            char *key_file_;

            if ((struck = strchr(optarg, ',')) != NULL) {
                *struck = 0;
                key_file_ = struck + 1;
            } else {
                key_file_ = optarg;
            }
            if (*optarg == 0 || *key_file_ == 0) {
                die(421, LOG_ERR, MSG_CONF_ERR ": TLS");
            }
            if ((cert_file = strdup(optarg)) == NULL) {
                die_mem();
            }
            if ((key_file = strdup(key_file_)) == NULL) {
                die_mem();
            }
            break;
        }
        case '3':
            tls_extcert_parse(optarg);
            use_extcert++;
            break;
        case 'Y': {
            if ((enforce_tls_auth = atoi(optarg)) < 0 || enforce_tls_auth > 3) {
                die(421, LOG_ERR, MSG_CONF_ERR ": TLS");
            }
            break;
        }
        case 'J': {
            while (*optarg == '-') {
                if (strncmp(optarg, "-S:", sizeof "-S:" - (size_t) 1U) == 0) {
                    optarg += sizeof "-S:" - (size_t) 1U;
                } else if (strncmp(optarg, "-C:", sizeof "-C:" - (size_t) 1U) == 0) {
                    optarg += sizeof "-C:" - (size_t) 1U;
                    ssl_verify_client_cert = 1;
                }
            }
            if ((tlsciphersuite = strdup(optarg)) == NULL) {
                die_mem();
            }
            break;
        }
#endif
        case 'e': {
            anon_only = 1;
            break;
        }
        case 'E': {
            anon_only = -1;
            break;
        }
#ifdef COOKIE
        case 'F': {
# ifdef BANNER_ENVIRON
            free(fortunes_file);
# endif
            fortunes_file = strdup(optarg);
            break;
        }
#endif
        case 'f': {
            int n = 0;

            if (strcasecmp(optarg, "none") == 0) {
                no_syslog = 1;
                break;
            }
            while (facilitynames[n].c_name &&
                   strcasecmp(facilitynames[n].c_name, optarg) != 0) {
                n++;
            }
            if (facilitynames[n].c_name) {
                syslog_facility = facilitynames[n].c_val;
            } else {
                logfile(LOG_ERR,
                        MSG_CONF_ERR ": " MSG_ILLEGAL_FACILITY ": %s", optarg);
            }
            break;
        }
        case 'l': {
            const Authentication *auth_list_pnt = auth_list;
            const char *opt = optarg;
            Authentications *new_auth;
            size_t auth_name_len;

            for (;;) {
                auth_name_len = strlen(auth_list_pnt->name);
                if (strncasecmp(opt, auth_list_pnt->name,
                                auth_name_len) == 0) {
                    char *file = NULL;

                    opt += auth_name_len;
                    if (*opt == ':') {
                        opt++;
                        if (*opt != 0) {
                            if ((file = strdup(opt)) == NULL) {
                                die_mem();
                            }
                        }
                    }
                    if (auth_list_pnt->parse != NULL) {
                        auth_list_pnt->parse(file);
                    }
                    if ((new_auth = malloc(sizeof *new_auth)) == NULL) {
                        die_mem();
                    }
                    new_auth->auth = auth_list_pnt;
                    new_auth->conf_file = file;
                    new_auth->next = NULL;
                    if (last_authentications == NULL) {
                        first_authentications = new_auth;
                    } else {
                        last_authentications->next = new_auth;
                    }
                    last_authentications = new_auth;

                    break;
                }
                auth_list_pnt++;
                if (auth_list_pnt->name == NULL) {
                    die(421, LOG_ERR, MSG_AUTH_UNKNOWN ": %s", opt);
                }
            }

            break;
        }
        case 'm': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            maxload = strtod(nptr, &endptr);
            if (!nptr || !*nptr || !endptr || *endptr || maxload <= 0.0) {
                die(421, LOG_ERR, MSG_CONF_ERR ": "
                    MSG_ILLEGAL_LOAD_LIMIT ": %s" , optarg);
            }
            break;
        }
        case 'M': {
            allow_anon_mkdir = 1;
            break;
        }
        case 'N': {
            disallow_passive = 1;
            break;
        }
#if defined(WITH_UPLOAD_SCRIPT)
        case 'o': {
            do_upload_script = 1;
            break;
        }
#endif
#ifdef WITH_ALTLOG
        case 'O': {
            char *optarg_copy;
            char *delpoint;

            if ((optarg_copy = strdup(optarg)) == NULL) {
                die_mem();
            }
            if ((delpoint = strchr(optarg_copy, ALTLOG_DELIMITER)) == NULL) {
                altlog_format = ALTLOG_DEFAULT;
                delpoint = optarg_copy;
            } else {
                const AltLogPrefixes *altlogprefixes_pnt = altlogprefixes;

                *delpoint++ = 0;
                do {
                    if (strcasecmp(optarg_copy,
                                   altlogprefixes_pnt->prefix) == 0) {
                        altlog_format = altlogprefixes_pnt->format;
                        break;
                    }
                    altlogprefixes_pnt++;
                } while (altlogprefixes_pnt->prefix != NULL);
                if (altlog_format == ALTLOG_NONE) {
                    die(421, LOG_ERR,
                        MSG_CONF_ERR ": " MSG_UNKNOWN_ALTLOG ": %s",
                        optarg_copy);
                }
            }
            if (*delpoint != '/') {
                die(421, LOG_ERR,
                    MSG_CONF_ERR ": " MSG_SANITY_FILE_FAILURE,
                    delpoint);
            }
            if (altlog_filename == NULL &&
                (altlog_filename = strdup(delpoint)) == NULL) {
                die_mem();
            }
            (void) free(optarg_copy);
            break;
        }
#endif
        case 'p': {
            int ret;

            ret = sscanf(optarg, "%u:%u", &firstport, &lastport);
            if (ret != 2 ||
                firstport < 1024U || lastport > 65535U
                || lastport < firstport) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_PORTS_RANGE ": %s" , optarg);
            }
            break;
        }
        case 'L': {
            int ret;

            ret = sscanf(optarg, "%u:%u", &max_ls_files, &max_ls_depth);
            if (ret != 2 ||
                max_ls_files < 1U || max_ls_depth < 1U) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_LS_LIMITS ": %s" , optarg);
            }
            break;
        }
#ifdef QUOTAS
        case 'n': {
            int ret;

            ret = sscanf(optarg, "%llu:%llu",
                         &user_quota_files, &user_quota_size);
            if (ret != 2) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_QUOTA ": %s" , optarg);
            }
            user_quota_size *= (1024ULL * 1024ULL);
            break;
        }
#endif
        case 'P': {
            if (force_passive_ip_s == NULL &&
                (force_passive_ip_s = strdup(optarg)) == NULL) {
                die_mem();
            }
            break;
        }
#ifdef RATIOS
        case 'q':
        case 'Q': {
            int ret;

            ret = sscanf(optarg, "%u:%u", &ratio_upload, &ratio_download);
            if (ret != 2 ||
                ratio_upload < 1U || ratio_download < 1U) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_RATIO ": %s" , optarg);
            }
            if (fodder == 'Q') {
                ratio_for_non_anon = 1;
            }
            break;
        }
#endif
        case 'r': {
            autorename = 1;
            break;
        }
        case 'R': {
            nochmod = 1;
            break;
        }
        case 'K': {
            keepallfiles = 1;
            break;
        }
#ifndef NO_STANDALONE
        case 'g': {
            if ((pid_file = strdup(optarg)) == NULL) {
                die_mem();
            }
            break;
        }
#endif
        case 'G': {
            disallow_rename = 1;
            break;
        }
        case 'H': {
            resolve_hostnames = 0;
            break;
        }
        case 'I': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            idletime = strtoul(nptr, &endptr, 0) * 60UL;
            if (idletime <= 0) {
                idletime = DEFAULT_IDLE;
            }
            break;
        }
        case 'i': {
            anon_noupload = 1;
            break;
        }
        case 'j': {
            create_home = 1;
            break;
        }
        case 'k': {
            const char *nptr;
            char *endptr;

            nptr = optarg;
            endptr = NULL;
            maxdiskusagepct = 1.0 - (strtod(nptr, &endptr) / 100.0);
            if (maxdiskusagepct >= 1.0 || maxdiskusagepct < 0.0) {
                maxdiskusagepct = 0.0;
            }
            break;
        }
        case 'u': {
            const char *nptr;
            char *endptr;
            long tmp;

            nptr = optarg;
            endptr = NULL;
            tmp = strtol(nptr, &endptr, 10);
            if (!nptr || !*nptr || !endptr || *endptr || tmp < 0) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_UID_LIMIT ": %s" , optarg);
            }
            useruid = (uid_t) tmp;
            break;
        }
        case 'U': {
            char *optarg_copy;
            char *struck;
            const char *tr_umask = NULL;
            const char *tr_umask_d = NULL;

            if ((optarg_copy = strdup(optarg)) == NULL) {
                die_mem();
            }
            if ((struck = strchr(optarg_copy, ':')) != NULL) {
                *struck = 0;
                if (*optarg_copy != 0) {
                    tr_umask = optarg_copy;
                }
                if (struck[1] != 0) {
                    tr_umask_d = &struck[1];
                }
            } else {
                tr_umask = tr_umask_d = optarg_copy;
            }
            if ((tr_umask == NULL || *tr_umask == 0) &&
                (tr_umask_d == NULL || *tr_umask_d == 0)) {
                bad_umask:
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_UMASK ": %s",
                    optarg_copy);
            }
            if (tr_umask != NULL) {
                if ((u_mask =
                     strtoul(tr_umask, NULL, 8)) > 0777) {
                    goto bad_umask;
                }
            }
            if (tr_umask_d != NULL) {
                if ((u_mask_d =
                     strtoul(tr_umask_d, NULL, 8)) > 0777) {
                    goto bad_umask;
                }
            }
            (void) free(optarg_copy);
            break;
        }
#ifdef WITH_VIRTUAL_HOSTS
        case 'V': {
            if (trustedip == NULL &&
                (trustedip = malloc(sizeof *trustedip)) == NULL) {
                die_mem();
            }
            if (generic_aton(optarg, trustedip) != 0) {
                die(421, LOG_ERR, MSG_CONF_ERR ": " MSG_ILLEGAL_TRUSTED_IP);
            }
            break;
        }
#endif
#ifdef WITH_BONJOUR
        case 'v': {
            char *rdvname;
            char *end;

            if ((rdvname = strdup(optarg)) == NULL) {
                die_mem();
            }
            doregistration(rdvname, strtoul(standalone_port, &end, 10));
            break;
        }
#endif
        case 'h': {
#ifndef NON_ROOT_FTP
            if (geteuid() == (uid_t) 0)
#endif
            {
                puts(PACKAGE " v" VERSION VERSION_PRIVSEP "\n");
            }
#ifndef NO_GETOPT_LONG
            {
                const struct option *options = long_options;

                do {
                    printf("-%c\t--%s\t%s\n", options->val, options->name,
                           options->has_arg ? "<opt>" : "");
                    options++;
                } while (options->name != NULL);
            }
#endif
            exit(EXIT_SUCCESS);
        }
        default:
            die(421, LOG_ERR, MSG_ILLEGAL_OPTION);
        }
    }
    if (optind < argc) {
        die(421, LOG_ERR, MSG_INVALID_ARGUMENT, argv[optind]);
    }
    if (first_authentications == NULL) {
        if ((first_authentications =
             malloc(sizeof *first_authentications)) == NULL) {
            die_mem();
        }
        first_authentications->auth = DEFAULT_AUTHENTICATION;
        first_authentications->conf_file = NULL;
        first_authentications->next = NULL;
    }
#ifndef NO_STANDALONE
    dodaemonize();
#endif
#ifndef SAVE_DESCRIPTORS
    if (no_syslog == 0 && (log_pid || syslog_facility != DEFAULT_FACILITY)) {
        closelog();
        openlog("pure-ftpd", LOG_NDELAY | log_pid, syslog_facility);
    }
#endif
    (void) umask((mode_t) 0);
    clearargs(argc, argv);
    idletime_noop = (double) idletime * 2.0;
    if (firstport) {
        unsigned int portmax;

        portmax = (lastport - firstport + 1) / 2;
        if (!maxusers || maxusers > portmax) {
            maxusers = portmax;    /* ... so we don't run out of ports */
        }
    }
    if (bypass_ipv6 == 0) {
        check_ipv6_support();
    }
#if defined(WITH_UPLOAD_SCRIPT)
    if (do_upload_script != 0) {
        upload_pipe_open();
    }
#endif
#ifdef WITH_DIRALIASES
    if (init_aliases() != 0) {
        logfile(LOG_ERR, MSG_ALIASES_BROKEN_FILE);
    }
#endif
#ifdef WITH_TLS
    if (enforce_tls_auth > 0) {
        (void) tls_init_library();
    }
#endif
#if !defined(NO_STANDALONE) && !defined(NO_INETD)
    if (check_standalone() != 0) {
        standalone_server();
    } else {
        doit();
    }
#elif !defined(NO_STANDALONE) && defined(NO_INETD)
    standalone_server();
#elif defined(NO_STANDALONE) && !defined(NO_INETD)
    doit();
#else
# error Configuration error
#endif

#ifdef WITH_UPLOAD_SCRIPT
    upload_pipe_close();
#endif
    {
        Authentications *auth_scan = first_authentications;
        Authentications *previous;

        while (auth_scan != NULL) {
            if (auth_scan->auth->exit != NULL) {
                auth_scan->auth->exit();
            }
            free(auth_scan->conf_file);
            previous = auth_scan;
            auth_scan = auth_scan->next;
            free(previous);
        }
    }
    first_authentications = last_authentications = NULL;
    free(trustedip);
#ifndef NO_STANDALONE
    iptrack_free();
    unlink(pid_file);
#endif
    closelog();
#ifdef WITH_TLS
    tls_free_library();
    tls_extcert_exit();
    free((void *) client_sni_name);
#endif
    alt_arc4random_close();

    _EXIT(EXIT_SUCCESS);

    return 0;
}