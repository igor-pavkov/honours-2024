static Token *expand_smacro(Token * tline)
{
    Token *t, *tt, *mstart, **tail, *thead;
    SMacro *head = NULL, *m;
    Token **params;
    int *paramsize;
    unsigned int nparam, sparam;
    int brackets;
    Token *org_tline = tline;
    Context *ctx;
    const char *mname;
    int deadman = DEADMAN_LIMIT;
    bool expanded;

    /*
     * Trick: we should avoid changing the start token pointer since it can
     * be contained in "next" field of other token. Because of this
     * we allocate a copy of first token and work with it; at the end of
     * routine we copy it back
     */
    if (org_tline) {
        tline = new_Token(org_tline->next, org_tline->type,
                          org_tline->text, 0);
        tline->a.mac = org_tline->a.mac;
        nasm_free(org_tline->text);
        org_tline->text = NULL;
    }

    expanded = true;            /* Always expand %+ at least once */

again:
    thead = NULL;
    tail = &thead;

    while (tline) {             /* main token loop */
        if (!--deadman) {
            nasm_error(ERR_NONFATAL, "interminable macro recursion");
            goto err;
        }

        if ((mname = tline->text)) {
            /* if this token is a local macro, look in local context */
            if (tline->type == TOK_ID) {
                head = (SMacro *)hash_findix(&smacros, mname);
            } else if (tline->type == TOK_PREPROC_ID) {
                ctx = get_ctx(mname, &mname);
                head = ctx ? (SMacro *)hash_findix(&ctx->localmac, mname) : NULL;
            } else
                head = NULL;

            /*
             * We've hit an identifier. As in is_mmacro below, we first
             * check whether the identifier is a single-line macro at
             * all, then think about checking for parameters if
             * necessary.
             */
            list_for_each(m, head)
                if (!mstrcmp(m->name, mname, m->casesense))
                    break;
            if (m) {
                mstart = tline;
                params = NULL;
                paramsize = NULL;
                if (m->nparam == 0) {
                    /*
                     * Simple case: the macro is parameterless. Discard the
                     * one token that the macro call took, and push the
                     * expansion back on the to-do stack.
                     */
                    if (!m->expansion) {
                        if (!strcmp("__FILE__", m->name)) {
                            const char *file = src_get_fname();
                            /* nasm_free(tline->text); here? */
                            tline->text = nasm_quote(file, strlen(file));
                            tline->type = TOK_STRING;
                            continue;
                        }
                        if (!strcmp("__LINE__", m->name)) {
                            nasm_free(tline->text);
                            make_tok_num(tline, src_get_linnum());
                            continue;
                        }
                        if (!strcmp("__BITS__", m->name)) {
                            nasm_free(tline->text);
                            make_tok_num(tline, globalbits);
                            continue;
                        }
                        tline = delete_Token(tline);
                        continue;
                    }
                } else {
                    /*
                     * Complicated case: at least one macro with this name
                     * exists and takes parameters. We must find the
                     * parameters in the call, count them, find the SMacro
                     * that corresponds to that form of the macro call, and
                     * substitute for the parameters when we expand. What a
                     * pain.
                     */
                    /*tline = tline->next;
                      skip_white_(tline); */
                    do {
                        t = tline->next;
                        while (tok_type_(t, TOK_SMAC_END)) {
                            t->a.mac->in_progress = false;
                            t->text = NULL;
                            t = tline->next = delete_Token(t);
                        }
                        tline = t;
                    } while (tok_type_(tline, TOK_WHITESPACE));
                    if (!tok_is_(tline, "(")) {
                        /*
                         * This macro wasn't called with parameters: ignore
                         * the call. (Behaviour borrowed from gnu cpp.)
                         */
                        tline = mstart;
                        m = NULL;
                    } else {
                        int paren = 0;
                        int white = 0;
                        brackets = 0;
                        nparam = 0;
                        sparam = PARAM_DELTA;
                        params = nasm_malloc(sparam * sizeof(Token *));
                        params[0] = tline->next;
                        paramsize = nasm_malloc(sparam * sizeof(int));
                        paramsize[0] = 0;
                        while (true) {  /* parameter loop */
                            /*
                             * For some unusual expansions
                             * which concatenates function call
                             */
                            t = tline->next;
                            while (tok_type_(t, TOK_SMAC_END)) {
                                t->a.mac->in_progress = false;
                                t->text = NULL;
                                t = tline->next = delete_Token(t);
                            }
                            tline = t;

                            if (!tline) {
                                nasm_error(ERR_NONFATAL,
                                      "macro call expects terminating `)'");
                                break;
                            }
                            if (tline->type == TOK_WHITESPACE
                                && brackets <= 0) {
                                if (paramsize[nparam])
                                    white++;
                                else
                                    params[nparam] = tline->next;
                                continue;       /* parameter loop */
                            }
                            if (tline->type == TOK_OTHER
                                && tline->text[1] == 0) {
                                char ch = tline->text[0];
                                if (ch == ',' && !paren && brackets <= 0) {
                                    if (++nparam >= sparam) {
                                        sparam += PARAM_DELTA;
                                        params = nasm_realloc(params,
                                                        sparam * sizeof(Token *));
                                        paramsize = nasm_realloc(paramsize,
                                                        sparam * sizeof(int));
                                    }
                                    params[nparam] = tline->next;
                                    paramsize[nparam] = 0;
                                    white = 0;
                                    continue;   /* parameter loop */
                                }
                                if (ch == '{' &&
                                    (brackets > 0 || (brackets == 0 &&
                                                      !paramsize[nparam])))
                                {
                                    if (!(brackets++)) {
                                        params[nparam] = tline->next;
                                        continue;       /* parameter loop */
                                    }
                                }
                                if (ch == '}' && brackets > 0)
                                    if (--brackets == 0) {
                                        brackets = -1;
                                        continue;       /* parameter loop */
                                    }
                                if (ch == '(' && !brackets)
                                    paren++;
                                if (ch == ')' && brackets <= 0)
                                    if (--paren < 0)
                                        break;
                            }
                            if (brackets < 0) {
                                brackets = 0;
                                nasm_error(ERR_NONFATAL, "braces do not "
                                      "enclose all of macro parameter");
                            }
                            paramsize[nparam] += white + 1;
                            white = 0;
                        }       /* parameter loop */
                        nparam++;
                        while (m && (m->nparam != nparam ||
                                     mstrcmp(m->name, mname,
                                             m->casesense)))
                            m = m->next;
                        if (!m)
                            nasm_error(ERR_WARNING|ERR_PASS1|ERR_WARN_MNP,
                                  "macro `%s' exists, "
                                  "but not taking %d parameters",
                                  mstart->text, nparam);
                    }
                }
                if (m && m->in_progress)
                    m = NULL;
                if (!m) {       /* in progess or didn't find '(' or wrong nparam */
                    /*
                     * Design question: should we handle !tline, which
                     * indicates missing ')' here, or expand those
                     * macros anyway, which requires the (t) test a few
                     * lines down?
                     */
                    nasm_free(params);
                    nasm_free(paramsize);
                    tline = mstart;
                } else {
                    /*
                     * Expand the macro: we are placed on the last token of the
                     * call, so that we can easily split the call from the
                     * following tokens. We also start by pushing an SMAC_END
                     * token for the cycle removal.
                     */
                    t = tline;
                    if (t) {
                        tline = t->next;
                        t->next = NULL;
                    }
                    tt = new_Token(tline, TOK_SMAC_END, NULL, 0);
                    tt->a.mac = m;
                    m->in_progress = true;
                    tline = tt;
                    list_for_each(t, m->expansion) {
                        if (t->type >= TOK_SMAC_PARAM) {
                            Token *pcopy = tline, **ptail = &pcopy;
                            Token *ttt, *pt;
                            int i;

                            ttt = params[t->type - TOK_SMAC_PARAM];
                            i = paramsize[t->type - TOK_SMAC_PARAM];
                            while (--i >= 0) {
                                pt = *ptail = new_Token(tline, ttt->type,
                                                        ttt->text, 0);
                                ptail = &pt->next;
                                ttt = ttt->next;
                            }
                            tline = pcopy;
                        } else if (t->type == TOK_PREPROC_Q) {
                            tt = new_Token(tline, TOK_ID, mname, 0);
                            tline = tt;
                        } else if (t->type == TOK_PREPROC_QQ) {
                            tt = new_Token(tline, TOK_ID, m->name, 0);
                            tline = tt;
                        } else {
                            tt = new_Token(tline, t->type, t->text, 0);
                            tline = tt;
                        }
                    }

                    /*
                     * Having done that, get rid of the macro call, and clean
                     * up the parameters.
                     */
                    nasm_free(params);
                    nasm_free(paramsize);
                    free_tlist(mstart);
                    expanded = true;
                    continue;   /* main token loop */
                }
            }
        }

        if (tline->type == TOK_SMAC_END) {
            tline->a.mac->in_progress = false;
            tline = delete_Token(tline);
        } else {
            t = *tail = tline;
            tline = tline->next;
            t->a.mac = NULL;
            t->next = NULL;
            tail = &t->next;
        }
    }

    /*
     * Now scan the entire line and look for successive TOK_IDs that resulted
     * after expansion (they can't be produced by tokenize()). The successive
     * TOK_IDs should be concatenated.
     * Also we look for %+ tokens and concatenate the tokens before and after
     * them (without white spaces in between).
     */
    if (expanded) {
        const struct tokseq_match t[] = {
            {
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_PREPROC_ID),     /* head */
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_PREPROC_ID)  |
                PP_CONCAT_MASK(TOK_NUMBER)          /* tail */
            }
        };
        if (paste_tokens(&thead, t, ARRAY_SIZE(t), true)) {
            /*
             * If we concatenated something, *and* we had previously expanded
             * an actual macro, scan the lines again for macros...
             */
            tline = thead;
            expanded = false;
            goto again;
        }
    }

err:
    if (org_tline) {
        if (thead) {
            *org_tline = *thead;
            /* since we just gave text to org_line, don't free it */
            thead->text = NULL;
            delete_Token(thead);
        } else {
            /* the expression expanded to empty line;
               we can't return NULL for some reasons
               we just set the line to a single WHITESPACE token. */
            memset(org_tline, 0, sizeof(*org_tline));
            org_tline->text = NULL;
            org_tline->type = TOK_WHITESPACE;
        }
        thead = org_tline;
    }

    return thead;
}