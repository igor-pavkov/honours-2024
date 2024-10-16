static void process_response_file(const char *file, int pass)
{
    char str[2048];
    FILE *f = nasm_open_read(file, NF_TEXT);
    if (!f) {
        perror(file);
        exit(-1);
    }
    while (fgets(str, sizeof str, f)) {
        process_args(str, pass);
    }
    fclose(f);
}