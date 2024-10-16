static bool if_condition(Token * tline, enum preproc_token ct)
{
    enum pp_conditional i = PP_COND(ct);
    bool j;
    Token *t, *tt, **tptr, *origline;
    struct tokenval tokval;
    expr *evalresult;
    enum pp_token_type needtype;
    char *p;

    origline = tline;

    switch (i) {
    case PPC_IFCTX:
        j = false;              /* have we matched yet? */
        while (true) {
            skip_white_(tline);
            if (!tline)
                break;
            if (tline->type != TOK_ID) {
                nasm_error(ERR_NONFATAL,
                      "`%s' expects context identifiers", pp_directives[ct]);
                free_tlist(origline);
                return -1;
            }
            if (cstk && cstk->name && !nasm_stricmp(tline->text, cstk->name))
                j = true;
            tline = tline->next;
        }
        break;

    case PPC_IFDEF:
        j = false;              /* have we matched yet? */
        while (tline) {
            skip_white_(tline);
            if (!tline || (tline->type != TOK_ID &&
                           (tline->type != TOK_PREPROC_ID ||
                            tline->text[1] != '$'))) {
                nasm_error(ERR_NONFATAL,
                      "`%s' expects macro identifiers", pp_directives[ct]);
                goto fail;
            }
            if (smacro_defined(NULL, tline->text, 0, NULL, true))
                j = true;
            tline = tline->next;
        }
        break;

    case PPC_IFENV:
        tline = expand_smacro(tline);
        j = false;              /* have we matched yet? */
        while (tline) {
            skip_white_(tline);
            if (!tline || (tline->type != TOK_ID &&
                           tline->type != TOK_STRING &&
                           (tline->type != TOK_PREPROC_ID ||
                            tline->text[1] != '!'))) {
                nasm_error(ERR_NONFATAL,
                      "`%s' expects environment variable names",
                      pp_directives[ct]);
                goto fail;
            }
            p = tline->text;
            if (tline->type == TOK_PREPROC_ID)
                p += 2;         /* Skip leading %! */
            if (*p == '\'' || *p == '\"' || *p == '`')
                nasm_unquote_cstr(p, ct);
            if (getenv(p))
                j = true;
            tline = tline->next;
        }
        break;

    case PPC_IFIDN:
    case PPC_IFIDNI:
        tline = expand_smacro(tline);
        t = tt = tline;
        while (tok_isnt_(tt, ","))
            tt = tt->next;
        if (!tt) {
            nasm_error(ERR_NONFATAL,
                  "`%s' expects two comma-separated arguments",
                  pp_directives[ct]);
            goto fail;
        }
        tt = tt->next;
        j = true;               /* assume equality unless proved not */
        while ((t->type != TOK_OTHER || strcmp(t->text, ",")) && tt) {
            if (tt->type == TOK_OTHER && !strcmp(tt->text, ",")) {
                nasm_error(ERR_NONFATAL, "`%s': more than one comma on line",
                      pp_directives[ct]);
                goto fail;
            }
            if (t->type == TOK_WHITESPACE) {
                t = t->next;
                continue;
            }
            if (tt->type == TOK_WHITESPACE) {
                tt = tt->next;
                continue;
            }
            if (tt->type != t->type) {
                j = false;      /* found mismatching tokens */
                break;
            }
            /* When comparing strings, need to unquote them first */
            if (t->type == TOK_STRING) {
                size_t l1 = nasm_unquote(t->text, NULL);
                size_t l2 = nasm_unquote(tt->text, NULL);

                if (l1 != l2) {
                    j = false;
                    break;
                }
                if (mmemcmp(t->text, tt->text, l1, i == PPC_IFIDN)) {
                    j = false;
                    break;
                }
            } else if (mstrcmp(tt->text, t->text, i == PPC_IFIDN) != 0) {
                j = false;      /* found mismatching tokens */
                break;
            }

            t = t->next;
            tt = tt->next;
        }
        if ((t->type != TOK_OTHER || strcmp(t->text, ",")) || tt)
            j = false;          /* trailing gunk on one end or other */
        break;

    case PPC_IFMACRO:
    {
        bool found = false;
        MMacro searching, *mmac;

        skip_white_(tline);
        tline = expand_id(tline);
        if (!tok_type_(tline, TOK_ID)) {
            nasm_error(ERR_NONFATAL,
                  "`%s' expects a macro name", pp_directives[ct]);
            goto fail;
        }
        searching.name = nasm_strdup(tline->text);
        searching.casesense = true;
        searching.plus = false;
        searching.nolist = false;
        searching.in_progress = 0;
        searching.max_depth = 0;
        searching.rep_nest = NULL;
        searching.nparam_min = 0;
        searching.nparam_max = INT_MAX;
        tline = expand_smacro(tline->next);
        skip_white_(tline);
        if (!tline) {
        } else if (!tok_type_(tline, TOK_NUMBER)) {
            nasm_error(ERR_NONFATAL,
                  "`%s' expects a parameter count or nothing",
                  pp_directives[ct]);
        } else {
            searching.nparam_min = searching.nparam_max =
                readnum(tline->text, &j);
            if (j)
                nasm_error(ERR_NONFATAL,
                      "unable to parse parameter count `%s'",
                      tline->text);
        }
        if (tline && tok_is_(tline->next, "-")) {
            tline = tline->next->next;
            if (tok_is_(tline, "*"))
                searching.nparam_max = INT_MAX;
            else if (!tok_type_(tline, TOK_NUMBER))
                nasm_error(ERR_NONFATAL,
                      "`%s' expects a parameter count after `-'",
                      pp_directives[ct]);
            else {
                searching.nparam_max = readnum(tline->text, &j);
                if (j)
                    nasm_error(ERR_NONFATAL,
                          "unable to parse parameter count `%s'",
                          tline->text);
                if (searching.nparam_min > searching.nparam_max)
                    nasm_error(ERR_NONFATAL,
                          "minimum parameter count exceeds maximum");
            }
        }
        if (tline && tok_is_(tline->next, "+")) {
            tline = tline->next;
            searching.plus = true;
        }
        mmac = (MMacro *) hash_findix(&mmacros, searching.name);
        while (mmac) {
            if (!strcmp(mmac->name, searching.name) &&
                (mmac->nparam_min <= searching.nparam_max
                 || searching.plus)
                && (searching.nparam_min <= mmac->nparam_max
                    || mmac->plus)) {
                found = true;
                break;
            }
            mmac = mmac->next;
        }
        if (tline && tline->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after %%ifmacro ignored");
        nasm_free(searching.name);
        j = found;
        break;
    }

    case PPC_IFID:
        needtype = TOK_ID;
        goto iftype;
    case PPC_IFNUM:
        needtype = TOK_NUMBER;
        goto iftype;
    case PPC_IFSTR:
        needtype = TOK_STRING;
        goto iftype;

iftype:
        t = tline = expand_smacro(tline);

        while (tok_type_(t, TOK_WHITESPACE) ||
               (needtype == TOK_NUMBER &&
                tok_type_(t, TOK_OTHER) &&
                (t->text[0] == '-' || t->text[0] == '+') &&
                !t->text[1]))
            t = t->next;

        j = tok_type_(t, needtype);
        break;

    case PPC_IFTOKEN:
        t = tline = expand_smacro(tline);
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;

        j = false;
        if (t) {
            t = t->next;        /* Skip the actual token */
            while (tok_type_(t, TOK_WHITESPACE))
                t = t->next;
            j = !t;             /* Should be nothing left */
        }
        break;

    case PPC_IFEMPTY:
        t = tline = expand_smacro(tline);
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;

        j = !t;                 /* Should be empty */
        break;

    case PPC_IF:
        t = tline = expand_smacro(tline);
        tptr = &t;
        tokval.t_type = TOKEN_INVALID;
        evalresult = evaluate(ppscan, tptr, &tokval,
                              NULL, pass | CRITICAL, NULL);
        if (!evalresult)
            return -1;
        if (tokval.t_type)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after expression ignored");
        if (!is_simple(evalresult)) {
            nasm_error(ERR_NONFATAL,
                  "non-constant value given to `%s'", pp_directives[ct]);
            goto fail;
        }
        j = reloc_value(evalresult) != 0;
        break;

    default:
        nasm_error(ERR_FATAL,
              "preprocessor directive `%s' not yet implemented",
              pp_directives[ct]);
        goto fail;
    }

    free_tlist(origline);
    return j ^ PP_NEGATIVE(ct);

fail:
    free_tlist(origline);
    return -1;
}