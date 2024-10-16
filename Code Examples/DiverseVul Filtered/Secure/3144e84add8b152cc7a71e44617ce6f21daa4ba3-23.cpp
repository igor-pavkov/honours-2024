static char *read_line(void)
{
    unsigned int size, c, next;
    const unsigned int delta = 512;
    const unsigned int pad = 8;
    unsigned int nr_cont = 0;
    bool cont = false;
    char *buffer, *p;

    /* Standart macros set (predefined) goes first */
    p = line_from_stdmac();
    if (p)
        return p;

    size = delta;
    p = buffer = nasm_malloc(size);

    for (;;) {
        c = fgetc(istk->fp);
        if ((int)(c) == EOF) {
            p[0] = 0;
            break;
        }

        switch (c) {
        case '\r':
            next = fgetc(istk->fp);
            if (next != '\n')
                ungetc(next, istk->fp);
            if (cont) {
                cont = false;
                continue;
            }
            break;

        case '\n':
            if (cont) {
                cont = false;
                continue;
            }
            break;

        case '\\':
            next = fgetc(istk->fp);
            ungetc(next, istk->fp);
            if (next == '\r' || next == '\n') {
                cont = true;
                nr_cont++;
                continue;
            }
            break;
        }

        if (c == '\r' || c == '\n') {
            *p++ = 0;
            break;
        }

        if (p >= (buffer + size - pad)) {
            buffer = nasm_realloc(buffer, size + delta);
            p = buffer + size - pad;
            size += delta;
        }

        *p++ = (unsigned char)c;
    }

    if (p == buffer) {
        nasm_free(buffer);
        return NULL;
    }

    src_set_linnum(src_get_linnum() + istk->lineinc +
                   (nr_cont * istk->lineinc));

    /*
     * Handle spurious ^Z, which may be inserted into source files
     * by some file transfer utilities.
     */
    buffer[strcspn(buffer, "\032")] = '\0';

    lfmt->line(LIST_READ, buffer);

    return buffer;
}