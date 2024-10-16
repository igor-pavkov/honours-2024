static void parse_cmdline(int argc, char **argv, int pass)
{
    FILE *rfile;
    char *envreal, *envcopy = NULL, *p;
    int i;

    /* Initialize all the warnings to their default state */
    for (i = 0; i < ERR_WARN_ALL; i++) {
        warning_state_init[i] = warning_state[i] =
	    warnings[i].enabled ? WARN_ST_ENABLED : 0;
    }

    /*
     * First, process the NASMENV environment variable.
     */
    envreal = getenv("NASMENV");
    if (envreal) {
        envcopy = nasm_strdup(envreal);
        process_args(envcopy, pass);
        nasm_free(envcopy);
    }

    /*
     * Now process the actual command line.
     */
    while (--argc) {
        bool advance;
        argv++;
        if (argv[0][0] == '@') {
            /*
             * We have a response file, so process this as a set of
             * arguments like the environment variable. This allows us
             * to have multiple arguments on a single line, which is
             * different to the -@resp file processing below for regular
             * NASM.
             */
            process_response_file(argv[0]+1, pass);
            argc--;
            argv++;
        }
        if (!stopoptions && argv[0][0] == '-' && argv[0][1] == '@') {
            p = get_param(argv[0], argc > 1 ? argv[1] : NULL, &advance);
            if (p) {
                rfile = nasm_open_read(p, NF_TEXT);
                if (rfile) {
                    process_respfile(rfile, pass);
                    fclose(rfile);
                } else
                    nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                                 "unable to open response file `%s'", p);
            }
        } else
            advance = process_arg(argv[0], argc > 1 ? argv[1] : NULL, pass);
        argv += advance, argc -= advance;
    }

    /*
     * Look for basic command line typos. This definitely doesn't
     * catch all errors, but it might help cases of fumbled fingers.
     */
    if (pass != 2)
        return;

    if (!inname)
        nasm_fatal(ERR_NOFILE | ERR_USAGE, "no input file specified");

    else if ((errname && !strcmp(inname, errname)) ||
             (outname && !strcmp(inname, outname)) ||
             (listname &&  !strcmp(inname, listname))  ||
             (depend_file && !strcmp(inname, depend_file)))
        nasm_fatal(ERR_USAGE, "will not overwrite input file");

    if (errname) {
        error_file = nasm_open_write(errname, NF_TEXT);
        if (!error_file) {
            error_file = stderr;        /* Revert to default! */
            nasm_fatal(ERR_NOFILE | ERR_USAGE,
                       "cannot open file `%s' for error messages",
                       errname);
        }
    }
}