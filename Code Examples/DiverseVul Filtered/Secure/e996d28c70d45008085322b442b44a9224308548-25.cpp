static void nasm_fputs(const char *line, FILE * outfile)
{
    if (outfile) {
        fputs(line, outfile);
        putc('\n', outfile);
    } else
        puts(line);
}