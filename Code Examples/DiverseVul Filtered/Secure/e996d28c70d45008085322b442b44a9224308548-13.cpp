int main(int argc, char **argv)
{
    StrList **depend_ptr;

    timestamp();

    iflag_set_default_cpu(&cpu);
    iflag_set_default_cpu(&cmd_cpu);

    pass0 = 0;
    want_usage = terminate_after_phase = false;
    nasm_set_verror(nasm_verror_gnu);

    error_file = stderr;

    tolower_init();
    src_init();

    /*
     * We must call init_labels() before the command line parsing,
     * because we may be setting prefixes/suffixes from the command
     * line.
     */
    init_labels();

    offsets = raa_init();
    forwrefs = saa_init((int32_t)sizeof(struct forwrefinfo));

    preproc = &nasmpp;
    operating_mode = OP_NORMAL;

    parse_cmdline(argc, argv, 1);
    if (terminate_after_phase) {
        if (want_usage)
            usage();
        return 1;
    }

    /*
     * Define some macros dependent on the runtime, but not
     * on the command line (as those are scanned in cmdline pass 2.)
     */
    preproc->init();
    define_macros_early();

    parse_cmdline(argc, argv, 2);
    if (terminate_after_phase) {
        if (want_usage)
            usage();
        return 1;
    }

    /* Save away the default state of warnings */
    memcpy(warning_state_init, warning_state, sizeof warning_state);

    if (!using_debug_info) {
        /* No debug info, redirect to the null backend (empty stubs) */
        dfmt = &null_debug_form;
    } else if (!debug_format) {
        /* Default debug format for this backend */
	dfmt = ofmt->default_dfmt;
    } else {
        dfmt = dfmt_find(ofmt, debug_format);
        if (!dfmt) {
            nasm_fatal(ERR_NOFILE | ERR_USAGE,
                       "unrecognized debug format `%s' for"
                       " output format `%s'",
                       debug_format, ofmt->shortname);
        }
    }

    if (ofmt->stdmac)
        preproc->extra_stdmac(ofmt->stdmac);

    /*
     * If no output file name provided and this
     * is a preprocess mode, we're perfectly
     * fine to output into stdout.
     */
    if (!outname) {
        if (!(operating_mode & OP_PREPROCESS))
            outname = filename_set_extension(inname, ofmt->extension);
    }

    /* define some macros dependent of command-line */
    define_macros_late();

    depend_ptr = (depend_file || (operating_mode & OP_DEPEND)) ? &depend_list : NULL;

    if (!depend_target)
        depend_target = quote_for_make(outname);

    if (operating_mode & OP_DEPEND) {
            char *line;

            if (depend_missing_ok)
                preproc->include_path(NULL);    /* "assume generated" */

            preproc->reset(inname, 0, depend_ptr);
            ofile = NULL;
            while ((line = preproc->getline()))
                nasm_free(line);
            preproc->cleanup(0);
    } else if (operating_mode & OP_PREPROCESS) {
            char *line;
            const char *file_name = NULL;
            int32_t prior_linnum = 0;
            int lineinc = 0;

            if (outname) {
                ofile = nasm_open_write(outname, NF_TEXT);
                if (!ofile)
                    nasm_fatal(ERR_NOFILE,
                                 "unable to open output file `%s'",
                                 outname);
            } else
                ofile = NULL;

            location.known = false;

            /* pass = 1; */
            preproc->reset(inname, 3, depend_ptr);

	    /* Revert all warnings to the default state */
	    memcpy(warning_state, warning_state_init, sizeof warning_state);

            while ((line = preproc->getline())) {
                /*
                 * We generate %line directives if needed for later programs
                 */
                int32_t linnum = prior_linnum += lineinc;
                int altline = src_get(&linnum, &file_name);
                if (altline) {
                    if (altline == 1 && lineinc == 1)
                        nasm_fputs("", ofile);
                    else {
                        lineinc = (altline != -1 || lineinc != 1);
                        fprintf(ofile ? ofile : stdout,
                                "%%line %"PRId32"+%d %s\n", linnum, lineinc,
                                file_name);
                    }
                    prior_linnum = linnum;
                }
                nasm_fputs(line, ofile);
                nasm_free(line);
            }
            preproc->cleanup(0);
            if (ofile)
                fclose(ofile);
            if (ofile && terminate_after_phase && !keep_all)
                remove(outname);
            ofile = NULL;
    }

    if (operating_mode & OP_NORMAL) {
        ofile = nasm_open_write(outname, (ofmt->flags & OFMT_TEXT) ? NF_TEXT : NF_BINARY);
        if (!ofile)
            nasm_fatal(ERR_NOFILE,
                       "unable to open output file `%s'", outname);

        ofmt->init();
        dfmt->init();

        assemble_file(inname, depend_ptr);

        if (!terminate_after_phase) {
            ofmt->cleanup();
            cleanup_labels();
            fflush(ofile);
            if (ferror(ofile)) {
                nasm_error(ERR_NONFATAL|ERR_NOFILE,
                           "write error on output file `%s'", outname);
                terminate_after_phase = true;
            }
        }

        if (ofile) {
            fclose(ofile);
            if (terminate_after_phase && !keep_all)
                remove(outname);
            ofile = NULL;
        }
    }

    if (depend_list && !terminate_after_phase)
        emit_dependencies(depend_list);

    if (want_usage)
        usage();

    raa_free(offsets);
    saa_free(forwrefs);
    eval_cleanup();
    stdscan_cleanup();
    src_free();

    return terminate_after_phase;
}