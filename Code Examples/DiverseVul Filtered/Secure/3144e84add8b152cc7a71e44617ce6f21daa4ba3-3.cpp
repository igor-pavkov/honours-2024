static Token *expand_id(Token * tline)
{
    Token *cur, *oldnext = NULL;

    if (!tline || !tline->next)
        return tline;

    cur = tline;
    while (cur->next &&
           (cur->next->type == TOK_ID ||
            cur->next->type == TOK_PREPROC_ID
            || cur->next->type == TOK_NUMBER))
        cur = cur->next;

    /* If identifier consists of just one token, don't expand */
    if (cur == tline)
        return tline;

    if (cur) {
        oldnext = cur->next;    /* Detach the tail past identifier */
        cur->next = NULL;       /* so that expand_smacro stops here */
    }

    tline = expand_smacro(tline);

    if (cur) {
        /* expand_smacro possibly changhed tline; re-scan for EOL */
        cur = tline;
        while (cur && cur->next)
            cur = cur->next;
        if (cur)
            cur->next = oldnext;
    }

    return tline;
}