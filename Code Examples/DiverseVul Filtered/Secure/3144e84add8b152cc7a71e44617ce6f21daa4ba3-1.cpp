static void count_mmac_params(Token * t, int *nparam, Token *** params)
{
    int paramsize, brace;

    *nparam = paramsize = 0;
    *params = NULL;
    while (t) {
        /* +1: we need space for the final NULL */
        if (*nparam+1 >= paramsize) {
            paramsize += PARAM_DELTA;
            *params = nasm_realloc(*params, sizeof(**params) * paramsize);
        }
        skip_white_(t);
        brace = 0;
        if (tok_is_(t, "{"))
            brace++;
        (*params)[(*nparam)++] = t;
        if (brace) {
            while (brace && (t = t->next) != NULL) {
                if (tok_is_(t, "{"))
                    brace++;
                else if (tok_is_(t, "}"))
                    brace--;
            }

            if (t) {
                /*
                 * Now we've found the closing brace, look further
                 * for the comma.
                 */
                t = t->next;
                skip_white_(t);
                if (tok_isnt_(t, ",")) {
                    nasm_error(ERR_NONFATAL,
                          "braces do not enclose all of macro parameter");
                    while (tok_isnt_(t, ","))
                        t = t->next;
                }
            }
        } else {
            while (tok_isnt_(t, ","))
                t = t->next;
        }
        if (t) {                /* got a comma/brace */
            t = t->next;        /* eat the comma */
        }
    }
}