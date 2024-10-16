static int ppscan(void *private_data, struct tokenval *tokval)
{
    Token **tlineptr = private_data;
    Token *tline;
    char ourcopy[MAX_KEYWORD+1], *p, *r, *s;

    do {
        tline = *tlineptr;
        *tlineptr = tline ? tline->next : NULL;
    } while (tline && (tline->type == TOK_WHITESPACE ||
                       tline->type == TOK_COMMENT));

    if (!tline)
        return tokval->t_type = TOKEN_EOS;

    tokval->t_charptr = tline->text;

    if (tline->text[0] == '$' && !tline->text[1])
        return tokval->t_type = TOKEN_HERE;
    if (tline->text[0] == '$' && tline->text[1] == '$' && !tline->text[2])
        return tokval->t_type = TOKEN_BASE;

    if (tline->type == TOK_ID) {
        p = tokval->t_charptr = tline->text;
        if (p[0] == '$') {
            tokval->t_charptr++;
            return tokval->t_type = TOKEN_ID;
        }

        for (r = p, s = ourcopy; *r; r++) {
            if (r >= p+MAX_KEYWORD)
                return tokval->t_type = TOKEN_ID; /* Not a keyword */
            *s++ = nasm_tolower(*r);
        }
        *s = '\0';
        /* right, so we have an identifier sitting in temp storage. now,
         * is it actually a register or instruction name, or what? */
        return nasm_token_hash(ourcopy, tokval);
    }

    if (tline->type == TOK_NUMBER) {
        bool rn_error;
        tokval->t_integer = readnum(tline->text, &rn_error);
        tokval->t_charptr = tline->text;
        if (rn_error)
            return tokval->t_type = TOKEN_ERRNUM;
        else
            return tokval->t_type = TOKEN_NUM;
    }

    if (tline->type == TOK_FLOAT) {
        return tokval->t_type = TOKEN_FLOAT;
    }

    if (tline->type == TOK_STRING) {
        char bq, *ep;

        bq = tline->text[0];
        tokval->t_charptr = tline->text;
        tokval->t_inttwo = nasm_unquote(tline->text, &ep);

        if (ep[0] != bq || ep[1] != '\0')
            return tokval->t_type = TOKEN_ERRSTR;
        else
            return tokval->t_type = TOKEN_STR;
    }

    if (tline->type == TOK_OTHER) {
        if (!strcmp(tline->text, "<<"))
            return tokval->t_type = TOKEN_SHL;
        if (!strcmp(tline->text, ">>"))
            return tokval->t_type = TOKEN_SHR;
        if (!strcmp(tline->text, "//"))
            return tokval->t_type = TOKEN_SDIV;
        if (!strcmp(tline->text, "%%"))
            return tokval->t_type = TOKEN_SMOD;
        if (!strcmp(tline->text, "=="))
            return tokval->t_type = TOKEN_EQ;
        if (!strcmp(tline->text, "<>"))
            return tokval->t_type = TOKEN_NE;
        if (!strcmp(tline->text, "!="))
            return tokval->t_type = TOKEN_NE;
        if (!strcmp(tline->text, "<="))
            return tokval->t_type = TOKEN_LE;
        if (!strcmp(tline->text, ">="))
            return tokval->t_type = TOKEN_GE;
        if (!strcmp(tline->text, "&&"))
            return tokval->t_type = TOKEN_DBL_AND;
        if (!strcmp(tline->text, "^^"))
            return tokval->t_type = TOKEN_DBL_XOR;
        if (!strcmp(tline->text, "||"))
            return tokval->t_type = TOKEN_DBL_OR;
    }

    /*
     * We have no other options: just return the first character of
     * the token text.
     */
    return tokval->t_type = tline->text[0];
}