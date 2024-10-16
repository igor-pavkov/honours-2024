static Token *expand_mmac_params_range(MMacro *mac, Token *tline, Token ***last)
{
    Token *t = tline, **tt, *tm, *head;
    char *pos;
    int fst, lst, j, i;

    pos = strchr(tline->text, ':');
    nasm_assert(pos);

    lst = atoi(pos + 1);
    fst = atoi(tline->text + 1);

    /*
     * only macros params are accounted so
     * if someone passes %0 -- we reject such
     * value(s)
     */
    if (lst == 0 || fst == 0)
        goto err;

    /* the values should be sane */
    if ((fst > (int)mac->nparam || fst < (-(int)mac->nparam)) ||
        (lst > (int)mac->nparam || lst < (-(int)mac->nparam)))
        goto err;

    fst = fst < 0 ? fst + (int)mac->nparam + 1: fst;
    lst = lst < 0 ? lst + (int)mac->nparam + 1: lst;

    /* counted from zero */
    fst--, lst--;

    /*
     * It will be at least one token. Note we
     * need to scan params until separator, otherwise
     * only first token will be passed.
     */
    tm = mac->params[(fst + mac->rotate) % mac->nparam];
    head = new_Token(NULL, tm->type, tm->text, 0);
    tt = &head->next, tm = tm->next;
    while (tok_isnt_(tm, ",")) {
        t = new_Token(NULL, tm->type, tm->text, 0);
        *tt = t, tt = &t->next, tm = tm->next;
    }

    if (fst < lst) {
        for (i = fst + 1; i <= lst; i++) {
            t = new_Token(NULL, TOK_OTHER, ",", 0);
            *tt = t, tt = &t->next;
            j = (i + mac->rotate) % mac->nparam;
            tm = mac->params[j];
            while (tok_isnt_(tm, ",")) {
                t = new_Token(NULL, tm->type, tm->text, 0);
                *tt = t, tt = &t->next, tm = tm->next;
            }
        }
    } else {
        for (i = fst - 1; i >= lst; i--) {
            t = new_Token(NULL, TOK_OTHER, ",", 0);
            *tt = t, tt = &t->next;
            j = (i + mac->rotate) % mac->nparam;
            tm = mac->params[j];
            while (tok_isnt_(tm, ",")) {
                t = new_Token(NULL, tm->type, tm->text, 0);
                *tt = t, tt = &t->next, tm = tm->next;
            }
        }
    }

    *last = tt;
    return head;

err:
    nasm_error(ERR_NONFATAL, "`%%{%s}': macro parameters out of range",
          &tline->text[1]);
    return tline;
}