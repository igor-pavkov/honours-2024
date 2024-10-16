static void help(const char xopt)
{
    int i;

    printf
        ("usage: nasm [-@ response file] [-o outfile] [-f format] "
         "[-l listfile]\n"
         "            [options...] [--] filename\n"
         "    or nasm -v (or --v) for version info\n\n"
         "\n"
         "Response files should contain command line parameters,\n"
         "one per line.\n"
         "\n"
         "    -t            assemble in SciTech TASM compatible mode\n");
    printf
        ("    -E (or -e)    preprocess only (writes output to stdout by default)\n"
         "    -a            don't preprocess (assemble only)\n"
         "    -M            generate Makefile dependencies on stdout\n"
         "    -MG           d:o, missing files assumed generated\n"
         "    -MF file      set Makefile dependency file\n"
         "    -MD file      assemble and generate dependencies\n"
         "    -MT file      dependency target name\n"
         "    -MQ file      dependency target name (quoted)\n"
         "    -MP           emit phony target\n\n"
         "    -Zfile        redirect error messages to file\n"
         "    -s            redirect error messages to stdout\n\n"
         "    -g            generate debugging information\n\n"
         "    -F format     select a debugging format\n\n"
         "    -gformat      same as -g -F format\n\n"
         "    -o outfile    write output to an outfile\n\n"
         "    -f format     select an output format\n\n"
         "    -l listfile   write listing to a listfile\n\n"
         "    -Ipath        add a pathname to the include file path\n");
    printf
        ("    -Olevel       optimize opcodes, immediates and branch offsets\n"
         "       -O0        no optimization\n"
         "       -O1        minimal optimization\n"
         "       -Ox        multipass optimization (default)\n"
         "    -Pfile        pre-include a file (also --include)\n"
         "    -Dmacro[=str] pre-define a macro\n"
         "    -Umacro       undefine a macro\n"
         "    -Xformat      specifiy error reporting format (gnu or vc)\n"
         "    -w+foo        enable warning foo (equiv. -Wfoo)\n"
         "    -w-foo        disable warning foo (equiv. -Wno-foo)\n"
         "    -w[+-]error[=foo]\n"
         "                  promote [specific] warnings to errors\n"
         "    -h            show invocation summary and exit (also --help)\n\n"
         "   --pragma str   pre-executes a specific %%pragma\n"
         "   --before str   add line (usually a preprocessor statement) before the input\n"
         "   --prefix str   prepend the given string to all the given string\n"
         "                  to all extern, common and global symbols (also --gprefix)\n"
         "   --postfix str  append the given string to all the given string\n"
         "                  to all extern, common and global symbols (also --gpostfix)\n"
         "   --lprefix str  prepend the given string to all other symbols\n"
         "   --lpostfix str append the given string to all other symbols\n"
         "   --keep-all     output files will not be removed even if an error happens\n"
         "   --limit-X val  set execution limit X\n");

    for (i = 0; i <= LIMIT_MAX; i++) {
        printf("                     %-15s %s (default ",
               limit_info[i].name, limit_info[i].help);
        if (nasm_limit[i] < LIMIT_MAX_VAL) {
            printf("%"PRId64")\n", nasm_limit[i]);
        } else {
            printf("unlimited)\n");
        }
    }

    printf("\nWarnings for the -W/-w options:\n");

    for (i = 0; i <= ERR_WARN_ALL; i++)
        printf("    %-23s %s%s\n",
               warnings[i].name, warnings[i].help,
               i == ERR_WARN_ALL ? "\n" :
               warnings[i].enabled ? " (default on)" :
               " (default off)");

    if (xopt == 'f') {
        printf("valid output formats for -f are"
               " (`*' denotes default):\n");
        ofmt_list(ofmt, stdout);
    } else {
        printf("For a list of valid output formats, use -hf.\n");
        printf("For a list of debug formats, use -f <format> -y.\n");
    }
}