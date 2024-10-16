static char *detoken(Token * tlist, bool expand_locals)
{
    Token *t;
    char *line, *p;
    const char *q;
    int len = 0;

    list_for_each(t, tlist) {
        if (t->type == TOK_PREPROC_ID && t->text[1] == '!') {
            char *v;
            char *q = t->text;

            v = t->text + 2;
            if (*v == '\'' || *v == '\"' || *v == '`') {
                size_t len = nasm_unquote(v, NULL);
                size_t clen = strlen(v);

                if (len != clen) {
                    nasm_error(ERR_NONFATAL | ERR_PASS1,
                          "NUL character in %%! string");
                    v = NULL;
                }
            }

            if (v) {
                char *p = getenv(v);
                if (!p) {
                    nasm_error(ERR_NONFATAL | ERR_PASS1,
                          "nonexistent environment variable `%s'", v);
                    /*
                     * FIXME We better should investigate if accessing
                     * ->text[1] without ->text[0] is safe enough.
                     */
                    t->text = nasm_zalloc(2);
                } else
                    t->text = nasm_strdup(p);
		nasm_free(q);
            }
        }

        /* Expand local macros here and not during preprocessing */
        if (expand_locals &&
            t->type == TOK_PREPROC_ID && t->text &&
            t->text[0] == '%' && t->text[1] == '$') {
            const char *q;
            char *p;
            Context *ctx = get_ctx(t->text, &q);
            if (ctx) {
                char buffer[40];
                snprintf(buffer, sizeof(buffer), "..@%"PRIu32".", ctx->number);
                p = nasm_strcat(buffer, q);
                nasm_free(t->text);
                t->text = p;
            }
        }
        if (t->type == TOK_WHITESPACE)
            len++;
        else if (t->text)
            len += strlen(t->text);
    }

    p = line = nasm_malloc(len + 1);

    list_for_each(t, tlist) {
        if (t->type == TOK_WHITESPACE) {
            *p++ = ' ';
        } else if (t->text) {
            q = t->text;
            while (*q)
                *p++ = *q++;
        }
    }
    *p = '\0';

    return line;
}