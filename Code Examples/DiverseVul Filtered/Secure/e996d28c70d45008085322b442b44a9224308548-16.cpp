static bool process_arg(char *p, char *q, int pass)
{
    char *param;
    bool advance = false;

    if (!p || !p[0])
        return false;

    if (p[0] == '-' && !stopoptions) {
        if (strchr("oOfpPdDiIlFXuUZwW", p[1])) {
            /* These parameters take values */
            if (!(param = get_param(p, q, &advance)))
                return advance;
        }

        switch (p[1]) {
        case 's':
            if (pass == 1)
                error_file = stdout;
            break;

        case 'o':       /* output file */
            if (pass == 2)
                copy_filename(&outname, param, "output");
            break;

        case 'f':       /* output format */
            if (pass == 1) {
                ofmt = ofmt_find(param, &ofmt_alias);
                if (!ofmt) {
                    nasm_fatal(ERR_NOFILE | ERR_USAGE,
                               "unrecognised output format `%s' - "
                               "use -hf for a list", param);
                }
            }
            break;

        case 'O':       /* Optimization level */
            if (pass == 2) {
                int opt;

                if (!*param) {
                    /* Naked -O == -Ox */
                    optimizing.level = MAX_OPTIMIZE;
                } else {
                    while (*param) {
                        switch (*param) {
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            opt = strtoul(param, &param, 10);

                            /* -O0 -> optimizing.level == -1, 0.98 behaviour */
                            /* -O1 -> optimizing.level == 0, 0.98.09 behaviour */
                            if (opt < 2)
                                optimizing.level = opt - 1;
                            else
                                optimizing.level = opt;
                            break;

                        case 'v':
                        case '+':
                        param++;
                        opt_verbose_info = true;
                        break;

                        case 'x':
                            param++;
                            optimizing.level = MAX_OPTIMIZE;
                            break;

                        default:
                            nasm_fatal(0,
                                       "unknown optimization option -O%c\n",
                                       *param);
                            break;
                        }
                    }
                    if (optimizing.level > MAX_OPTIMIZE)
                        optimizing.level = MAX_OPTIMIZE;
                }
            }
            break;

        case 'p':       /* pre-include */
        case 'P':
            if (pass == 2)
                preproc->pre_include(param);
            break;

        case 'd':       /* pre-define */
        case 'D':
            if (pass == 2)
                preproc->pre_define(param);
            break;

        case 'u':       /* un-define */
        case 'U':
            if (pass == 2)
                preproc->pre_undefine(param);
            break;

        case 'i':       /* include search path */
        case 'I':
            if (pass == 2)
                preproc->include_path(param);
            break;

        case 'l':       /* listing file */
            if (pass == 2)
                copy_filename(&listname, param, "listing");
            break;

        case 'Z':       /* error messages file */
            if (pass == 1)
                copy_filename(&errname, param, "error");
            break;

        case 'F':       /* specify debug format */
            if (pass == 2) {
                using_debug_info = true;
                debug_format = param;
            }
            break;

        case 'X':       /* specify error reporting format */
            if (pass == 1) {
                if (nasm_stricmp("vc", param) == 0)
                    nasm_set_verror(nasm_verror_vc);
                else if (nasm_stricmp("gnu", param) == 0)
                    nasm_set_verror(nasm_verror_gnu);
                else
                    nasm_fatal(ERR_NOFILE | ERR_USAGE,
                               "unrecognized error reporting format `%s'",
                               param);
            }
            break;

        case 'g':
            if (pass == 2) {
                using_debug_info = true;
                if (p[2])
                    debug_format = nasm_skip_spaces(p + 2);
            }
            break;

        case 'h':
            help(p[2]);
            exit(0);    /* never need usage message here */
            break;

        case 'y':
            printf("\nvalid debug formats for '%s' output format are"
                   " ('*' denotes default):\n", ofmt->shortname);
            dfmt_list(ofmt, stdout);
            exit(0);
            break;

        case 't':
            if (pass == 2)
                tasm_compatible_mode = true;
            break;

        case 'v':
            show_version();
            break;

        case 'e':       /* preprocess only */
        case 'E':
            if (pass == 1)
                operating_mode = OP_PREPROCESS;
            break;

        case 'a':       /* assemble only - don't preprocess */
            if (pass == 1)
                preproc = &preproc_nop;
            break;

        case 'w':
        case 'W':
            if (pass == 2) {
                if (!set_warning_status(param)) {
                    nasm_error(ERR_WARNING|ERR_NOFILE|ERR_WARN_UNK_WARNING,
			       "unknown warning option: %s", param);
                }
            }
        break;

        case 'M':
            if (pass == 1) {
                switch (p[2]) {
                case 'W':
                    quote_for_make = quote_for_wmake;
                    break;
                case 'D':
                case 'F':
                case 'T':
                case 'Q':
                    advance = true;
                    break;
                default:
                    break;
                }
            } else {
                switch (p[2]) {
                case 0:
                    operating_mode = OP_DEPEND;
                    break;
                case 'G':
                    operating_mode = OP_DEPEND;
                    depend_missing_ok = true;
                    break;
                case 'P':
                    depend_emit_phony = true;
                    break;
                case 'D':
                    operating_mode = OP_NORMAL;
                    depend_file = q;
                    advance = true;
                    break;
                case 'F':
                    depend_file = q;
                    advance = true;
                    break;
                case 'T':
                    depend_target = q;
                    advance = true;
                    break;
                case 'Q':
                    depend_target = quote_for_make(q);
                    advance = true;
                    break;
                case 'W':
                    /* handled in pass 1 */
                    break;
                default:
                    nasm_error(ERR_NONFATAL|ERR_NOFILE|ERR_USAGE,
                               "unknown dependency option `-M%c'", p[2]);
                    break;
                }
            }
            if (advance && (!q || !q[0])) {
                nasm_error(ERR_NONFATAL|ERR_NOFILE|ERR_USAGE,
                           "option `-M%c' requires a parameter", p[2]);
                break;
            }
            break;

        case '-':
            {
                const struct textargs *tx;
                size_t olen, plen;
                char *eqsave;

                p += 2;

                if (!*p) {        /* -- => stop processing options */
                    stopoptions = true;
                    break;
                }

                plen = strlen(p);
                for (tx = textopts; tx->label; tx++) {
                    olen = strlen(tx->label);

                    if (olen > plen)
                        continue;

                    if (nasm_memicmp(p, tx->label, olen))
                        continue;

                    if (tx->label[olen-1] == '-')
                        break;  /* Incomplete option */

                    if (!p[olen] || p[olen] == '=')
                        break;  /* Complete option */
                }

                if (!tx->label) {
                    nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                               "unrecognized option `--%s'", p);
                }

                eqsave = param = strchr(p+olen, '=');
                if (param)
                    *param++ = '\0';

                if (tx->need_arg) {
                    if (!param) {
                        param = q;
                        advance = true;
                    }

                    /* Note: a null string is a valid parameter */
                    if (!param) {
                        nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                                   "option `--%s' requires an argument",
                                   p);
                        break;
                    }
                } else {
                    if (param) {
                        nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                                   "option `--%s' does not take an argument",
                                   p);

                    }
                }

                switch (tx->opt) {
                case OPT_VERSION:
                    show_version();
                    break;
                case OPT_ABORT_ON_PANIC:
                    abort_on_panic = true;
                    break;
                case OPT_MANGLE:
                    if (pass == 2)
                        set_label_mangle(tx->pvt, param);
                    break;
                case OPT_INCLUDE:
                    if (pass == 2)
                        preproc->pre_include(q);
                    break;
                case OPT_PRAGMA:
                    if (pass == 2)
                        preproc->pre_command("pragma", param);
                    break;
                case OPT_BEFORE:
                    if (pass == 2)
                        preproc->pre_command(NULL, param);
                    break;
                case OPT_LIMIT:
                    if (pass == 2)
                        nasm_set_limit(p+olen, param);
                    break;
                case OPT_KEEP_ALL:
                    keep_all = true;
                    break;
                case OPT_HELP:
                    help(0);
                    exit(0);
                default:
                    panic();
                }

                if (eqsave)
                    *eqsave = '='; /* Restore = argument separator */

                break;
            }

        default:
            nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                       "unrecognised option `-%c'", p[1]);
            break;
        }
    } else if (pass == 2) {
        /* In theory we could allow multiple input files... */
        copy_filename(&inname, p, "input");
    }

    return advance;
}