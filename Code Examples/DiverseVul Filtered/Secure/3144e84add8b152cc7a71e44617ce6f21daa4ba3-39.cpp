static Token *expand_mmac_params(Token * tline)
{
    Token *t, *tt, **tail, *thead;
    bool changed = false;
    char *pos;

    tail = &thead;
    thead = NULL;

    while (tline) {
        if (tline->type == TOK_PREPROC_ID &&
            (((tline->text[1] == '+' || tline->text[1] == '-') && tline->text[2])   ||
              (tline->text[1] >= '0' && tline->text[1] <= '9')                      ||
               tline->text[1] == '%')) {
            char *text = NULL;
            int type = 0, cc;   /* type = 0 to placate optimisers */
            char tmpbuf[30];
            unsigned int n;
            int i;
            MMacro *mac;

            t = tline;
            tline = tline->next;

            mac = istk->mstk;
            while (mac && !mac->name)   /* avoid mistaking %reps for macros */
                mac = mac->next_active;
            if (!mac) {
                nasm_error(ERR_NONFATAL, "`%s': not in a macro call", t->text);
            } else {
                pos = strchr(t->text, ':');
                if (!pos) {
                    switch (t->text[1]) {
                        /*
                         * We have to make a substitution of one of the
                         * forms %1, %-1, %+1, %%foo, %0.
                         */
                    case '0':
                        type = TOK_NUMBER;
                        snprintf(tmpbuf, sizeof(tmpbuf), "%d", mac->nparam);
                        text = nasm_strdup(tmpbuf);
                        break;
                    case '%':
                        type = TOK_ID;
                        snprintf(tmpbuf, sizeof(tmpbuf), "..@%"PRIu64".",
                                 mac->unique);
                        text = nasm_strcat(tmpbuf, t->text + 2);
                        break;
                    case '-':
                        n = atoi(t->text + 2) - 1;
                        if (n >= mac->nparam)
                            tt = NULL;
                        else {
                            if (mac->nparam > 1)
                                n = (n + mac->rotate) % mac->nparam;
                            tt = mac->params[n];
                        }
                        cc = find_cc(tt);
                        if (cc == -1) {
                            nasm_error(ERR_NONFATAL,
                                  "macro parameter %d is not a condition code",
                                  n + 1);
                            text = NULL;
                        } else {
                            type = TOK_ID;
                            if (inverse_ccs[cc] == -1) {
                                nasm_error(ERR_NONFATAL,
                                      "condition code `%s' is not invertible",
                                      conditions[cc]);
                                text = NULL;
                            } else
                                text = nasm_strdup(conditions[inverse_ccs[cc]]);
                        }
                        break;
                    case '+':
                        n = atoi(t->text + 2) - 1;
                        if (n >= mac->nparam)
                            tt = NULL;
                        else {
                            if (mac->nparam > 1)
                                n = (n + mac->rotate) % mac->nparam;
                            tt = mac->params[n];
                        }
                        cc = find_cc(tt);
                        if (cc == -1) {
                            nasm_error(ERR_NONFATAL,
                                  "macro parameter %d is not a condition code",
                                  n + 1);
                            text = NULL;
                        } else {
                            type = TOK_ID;
                            text = nasm_strdup(conditions[cc]);
                        }
                        break;
                    default:
                        n = atoi(t->text + 1) - 1;
                        if (n >= mac->nparam)
                            tt = NULL;
                        else {
                            if (mac->nparam > 1)
                                n = (n + mac->rotate) % mac->nparam;
                            tt = mac->params[n];
                        }
                        if (tt) {
                            for (i = 0; i < mac->paramlen[n]; i++) {
                                *tail = new_Token(NULL, tt->type, tt->text, 0);
                                tail = &(*tail)->next;
                                tt = tt->next;
                            }
                        }
                        text = NULL;        /* we've done it here */
                        break;
                    }
                } else {
                    /*
                     * seems we have a parameters range here
                     */
                    Token *head, **last;
                    head = expand_mmac_params_range(mac, t, &last);
                    if (head != t) {
                        *tail = head;
                        *last = tline;
                        tline = head;
                        text = NULL;
                    }
                }
            }
            if (!text) {
                delete_Token(t);
            } else {
                *tail = t;
                tail = &t->next;
                t->type = type;
                nasm_free(t->text);
                t->text = text;
                t->a.mac = NULL;
            }
            changed = true;
            continue;
        } else if (tline->type == TOK_INDIRECT) {
            t = tline;
            tline = tline->next;
            tt = tokenize(t->text);
            tt = expand_mmac_params(tt);
            tt = expand_smacro(tt);
            *tail = tt;
            while (tt) {
                tt->a.mac = NULL; /* Necessary? */
                tail = &tt->next;
                tt = tt->next;
            }
            delete_Token(t);
            changed = true;
        } else {
            t = *tail = tline;
            tline = tline->next;
            t->a.mac = NULL;
            tail = &t->next;
        }
    }
    *tail = NULL;

    if (changed) {
        const struct tokseq_match t[] = {
            {
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_FLOAT),          /* head */
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_NUMBER)      |
                PP_CONCAT_MASK(TOK_FLOAT)       |
                PP_CONCAT_MASK(TOK_OTHER)           /* tail */
            },
            {
                PP_CONCAT_MASK(TOK_NUMBER),         /* head */
                PP_CONCAT_MASK(TOK_NUMBER)          /* tail */
            }
        };
        paste_tokens(&thead, t, ARRAY_SIZE(t), false);
    }

    return thead;
}