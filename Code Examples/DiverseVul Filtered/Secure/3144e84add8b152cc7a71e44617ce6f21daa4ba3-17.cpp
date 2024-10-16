static int do_directive(Token *tline, char **output)
{
    enum preproc_token i;
    int j;
    bool err;
    int nparam;
    bool nolist;
    bool casesense;
    int k, m;
    int offset;
    char *p, *pp;
    const char *found_path;
    const char *mname;
    Include *inc;
    Context *ctx;
    Cond *cond;
    MMacro *mmac, **mmhead;
    Token *t = NULL, *tt, *param_start, *macro_start, *last, **tptr, *origline;
    Line *l;
    struct tokenval tokval;
    expr *evalresult;
    MMacro *tmp_defining;       /* Used when manipulating rep_nest */
    int64_t count;
    size_t len;
    int severity;

    *output = NULL;             /* No output generated */
    origline = tline;

    skip_white_(tline);
    if (!tline || !tok_type_(tline, TOK_PREPROC_ID) ||
        (tline->text[1] == '%' || tline->text[1] == '$'
         || tline->text[1] == '!'))
        return NO_DIRECTIVE_FOUND;

    i = pp_token_hash(tline->text);

    /*
     * FIXME: We zap execution of PP_RMACRO, PP_IRMACRO, PP_EXITMACRO
     * since they are known to be buggy at moment, we need to fix them
     * in future release (2.09-2.10)
     */
    if (i == PP_RMACRO || i == PP_IRMACRO || i == PP_EXITMACRO) {
        nasm_error(ERR_NONFATAL, "unknown preprocessor directive `%s'",
              tline->text);
       return NO_DIRECTIVE_FOUND;
    }

    /*
     * If we're in a non-emitting branch of a condition construct,
     * or walking to the end of an already terminated %rep block,
     * we should ignore all directives except for condition
     * directives.
     */
    if (((istk->conds && !emitting(istk->conds->state)) ||
         (istk->mstk && !istk->mstk->in_progress)) && !is_condition(i)) {
        return NO_DIRECTIVE_FOUND;
    }

    /*
     * If we're defining a macro or reading a %rep block, we should
     * ignore all directives except for %macro/%imacro (which nest),
     * %endm/%endmacro, and (only if we're in a %rep block) %endrep.
     * If we're in a %rep block, another %rep nests, so should be let through.
     */
    if (defining && i != PP_MACRO && i != PP_IMACRO &&
        i != PP_RMACRO &&  i != PP_IRMACRO &&
        i != PP_ENDMACRO && i != PP_ENDM &&
        (defining->name || (i != PP_ENDREP && i != PP_REP))) {
        return NO_DIRECTIVE_FOUND;
    }

    if (defining) {
        if (i == PP_MACRO || i == PP_IMACRO ||
            i == PP_RMACRO || i == PP_IRMACRO) {
            nested_mac_count++;
            return NO_DIRECTIVE_FOUND;
        } else if (nested_mac_count > 0) {
            if (i == PP_ENDMACRO) {
                nested_mac_count--;
                return NO_DIRECTIVE_FOUND;
            }
        }
        if (!defining->name) {
            if (i == PP_REP) {
                nested_rep_count++;
                return NO_DIRECTIVE_FOUND;
            } else if (nested_rep_count > 0) {
                if (i == PP_ENDREP) {
                    nested_rep_count--;
                    return NO_DIRECTIVE_FOUND;
                }
            }
        }
    }

    switch (i) {
    case PP_INVALID:
        nasm_error(ERR_NONFATAL, "unknown preprocessor directive `%s'",
              tline->text);
        return NO_DIRECTIVE_FOUND;      /* didn't get it */

    case PP_PRAGMA:
        /*
         * %pragma namespace options...
         *
         * The namespace "preproc" is reserved for the preprocessor;
         * all other namespaces generate a [pragma] assembly directive.
         *
         * Invalid %pragmas are ignored and may have different
         * meaning in future versions of NASM.
         */
        tline = tline->next;
        skip_white_(tline);
        tline = expand_smacro(tline);
        if (tok_type_(tline, TOK_ID)) {
            if (!nasm_stricmp(tline->text, "preproc")) {
                /* Preprocessor pragma */
                do_pragma_preproc(tline);
            } else {
                /* Build the assembler directive */
                t = new_Token(NULL, TOK_OTHER, "[", 1);
                t->next = new_Token(NULL, TOK_ID, "pragma", 6);
                t->next->next = new_Token(tline, TOK_WHITESPACE, NULL, 0);
                tline = t;
                for (t = tline; t->next; t = t->next)
                    ;
                t->next = new_Token(NULL, TOK_OTHER, "]", 1);
                /* true here can be revisited in the future */
                *output = detoken(tline, true);
            }
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_STACKSIZE:
        /* Directive to tell NASM what the default stack size is. The
         * default is for a 16-bit stack, and this can be overriden with
         * %stacksize large.
         */
        tline = tline->next;
        if (tline && tline->type == TOK_WHITESPACE)
            tline = tline->next;
        if (!tline || tline->type != TOK_ID) {
            nasm_error(ERR_NONFATAL, "`%%stacksize' missing size parameter");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        if (nasm_stricmp(tline->text, "flat") == 0) {
            /* All subsequent ARG directives are for a 32-bit stack */
            StackSize = 4;
            StackPointer = "ebp";
            ArgOffset = 8;
            LocalOffset = 0;
        } else if (nasm_stricmp(tline->text, "flat64") == 0) {
            /* All subsequent ARG directives are for a 64-bit stack */
            StackSize = 8;
            StackPointer = "rbp";
            ArgOffset = 16;
            LocalOffset = 0;
        } else if (nasm_stricmp(tline->text, "large") == 0) {
            /* All subsequent ARG directives are for a 16-bit stack,
             * far function call.
             */
            StackSize = 2;
            StackPointer = "bp";
            ArgOffset = 4;
            LocalOffset = 0;
        } else if (nasm_stricmp(tline->text, "small") == 0) {
            /* All subsequent ARG directives are for a 16-bit stack,
             * far function call. We don't support near functions.
             */
            StackSize = 2;
            StackPointer = "bp";
            ArgOffset = 6;
            LocalOffset = 0;
        } else {
            nasm_error(ERR_NONFATAL, "`%%stacksize' invalid size type");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_ARG:
        /* TASM like ARG directive to define arguments to functions, in
         * the following form:
         *
         *      ARG arg1:WORD, arg2:DWORD, arg4:QWORD
         */
        offset = ArgOffset;
        do {
            char *arg, directive[256];
            int size = StackSize;

            /* Find the argument name */
            tline = tline->next;
            if (tline && tline->type == TOK_WHITESPACE)
                tline = tline->next;
            if (!tline || tline->type != TOK_ID) {
                nasm_error(ERR_NONFATAL, "`%%arg' missing argument parameter");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            arg = tline->text;

            /* Find the argument size type */
            tline = tline->next;
            if (!tline || tline->type != TOK_OTHER
                || tline->text[0] != ':') {
                nasm_error(ERR_NONFATAL,
                      "Syntax error processing `%%arg' directive");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            tline = tline->next;
            if (!tline || tline->type != TOK_ID) {
                nasm_error(ERR_NONFATAL, "`%%arg' missing size type parameter");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }

            /* Allow macro expansion of type parameter */
            tt = tokenize(tline->text);
            tt = expand_smacro(tt);
            size = parse_size(tt->text);
            if (!size) {
                nasm_error(ERR_NONFATAL,
                      "Invalid size type for `%%arg' missing directive");
                free_tlist(tt);
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            free_tlist(tt);

            /* Round up to even stack slots */
            size = ALIGN(size, StackSize);

            /* Now define the macro for the argument */
            snprintf(directive, sizeof(directive), "%%define %s (%s+%d)",
                     arg, StackPointer, offset);
            do_directive(tokenize(directive), output);
            offset += size;

            /* Move to the next argument in the list */
            tline = tline->next;
            if (tline && tline->type == TOK_WHITESPACE)
                tline = tline->next;
        } while (tline && tline->type == TOK_OTHER && tline->text[0] == ',');
        ArgOffset = offset;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_LOCAL:
        /* TASM like LOCAL directive to define local variables for a
         * function, in the following form:
         *
         *      LOCAL local1:WORD, local2:DWORD, local4:QWORD = LocalSize
         *
         * The '= LocalSize' at the end is ignored by NASM, but is
         * required by TASM to define the local parameter size (and used
         * by the TASM macro package).
         */
        offset = LocalOffset;
        do {
            char *local, directive[256];
            int size = StackSize;

            /* Find the argument name */
            tline = tline->next;
            if (tline && tline->type == TOK_WHITESPACE)
                tline = tline->next;
            if (!tline || tline->type != TOK_ID) {
                nasm_error(ERR_NONFATAL,
                      "`%%local' missing argument parameter");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            local = tline->text;

            /* Find the argument size type */
            tline = tline->next;
            if (!tline || tline->type != TOK_OTHER
                || tline->text[0] != ':') {
                nasm_error(ERR_NONFATAL,
                      "Syntax error processing `%%local' directive");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            tline = tline->next;
            if (!tline || tline->type != TOK_ID) {
                nasm_error(ERR_NONFATAL,
                      "`%%local' missing size type parameter");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }

            /* Allow macro expansion of type parameter */
            tt = tokenize(tline->text);
            tt = expand_smacro(tt);
            size = parse_size(tt->text);
            if (!size) {
                nasm_error(ERR_NONFATAL,
                      "Invalid size type for `%%local' missing directive");
                free_tlist(tt);
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            free_tlist(tt);

            /* Round up to even stack slots */
            size = ALIGN(size, StackSize);

            offset += size;     /* Negative offset, increment before */

            /* Now define the macro for the argument */
            snprintf(directive, sizeof(directive), "%%define %s (%s-%d)",
                     local, StackPointer, offset);
            do_directive(tokenize(directive), output);

            /* Now define the assign to setup the enter_c macro correctly */
            snprintf(directive, sizeof(directive),
                     "%%assign %%$localsize %%$localsize+%d", size);
            do_directive(tokenize(directive), output);

            /* Move to the next argument in the list */
            tline = tline->next;
            if (tline && tline->type == TOK_WHITESPACE)
                tline = tline->next;
        } while (tline && tline->type == TOK_OTHER && tline->text[0] == ',');
        LocalOffset = offset;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_CLEAR:
        if (tline->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after `%%clear' ignored");
        free_macros();
        init_macros();
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_DEPEND:
        t = tline->next = expand_smacro(tline->next);
        skip_white_(t);
        if (!t || (t->type != TOK_STRING &&
                   t->type != TOK_INTERNAL_STRING)) {
            nasm_error(ERR_NONFATAL, "`%%depend' expects a file name");
            free_tlist(origline);
            return DIRECTIVE_FOUND;     /* but we did _something_ */
        }
        if (t->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after `%%depend' ignored");
        p = t->text;
        if (t->type != TOK_INTERNAL_STRING)
            nasm_unquote_cstr(p, i);
        nasm_add_string_to_strlist(dephead, p);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_INCLUDE:
        t = tline->next = expand_smacro(tline->next);
        skip_white_(t);

        if (!t || (t->type != TOK_STRING &&
                   t->type != TOK_INTERNAL_STRING)) {
            nasm_error(ERR_NONFATAL, "`%%include' expects a file name");
            free_tlist(origline);
            return DIRECTIVE_FOUND;     /* but we did _something_ */
        }
        if (t->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after `%%include' ignored");
        p = t->text;
        if (t->type != TOK_INTERNAL_STRING)
            nasm_unquote_cstr(p, i);
        inc = nasm_malloc(sizeof(Include));
        inc->next = istk;
        inc->conds = NULL;
        found_path = NULL;
        inc->fp = inc_fopen(p, dephead, &found_path,
                            pass == 0 ? INC_OPTIONAL : INC_NEEDED, NF_TEXT);
        if (!inc->fp) {
            /* -MG given but file not found */
            nasm_free(inc);
        } else {
            inc->fname = src_set_fname(found_path ? found_path : p);
            inc->lineno = src_set_linnum(0);
            inc->lineinc = 1;
            inc->expansion = NULL;
            inc->mstk = NULL;
            istk = inc;
            lfmt->uplevel(LIST_INCLUDE);
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_USE:
    {
        static macros_t *use_pkg;
        const char *pkg_macro = NULL;

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);

        if (!tline || (tline->type != TOK_STRING &&
                       tline->type != TOK_INTERNAL_STRING &&
                       tline->type != TOK_ID)) {
            nasm_error(ERR_NONFATAL, "`%%use' expects a package name");
            free_tlist(origline);
            return DIRECTIVE_FOUND;     /* but we did _something_ */
        }
        if (tline->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after `%%use' ignored");
        if (tline->type == TOK_STRING)
            nasm_unquote_cstr(tline->text, i);
        use_pkg = nasm_stdmac_find_package(tline->text);
        if (!use_pkg)
            nasm_error(ERR_NONFATAL, "unknown `%%use' package: %s", tline->text);
        else
            pkg_macro = (char *)use_pkg + 1; /* The first string will be <%define>__USE_*__ */
        if (use_pkg && ! smacro_defined(NULL, pkg_macro, 0, NULL, true)) {
            /* Not already included, go ahead and include it */
            stdmacpos = use_pkg;
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;
    }
    case PP_PUSH:
    case PP_REPL:
    case PP_POP:
        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (tline) {
            if (!tok_type_(tline, TOK_ID)) {
                nasm_error(ERR_NONFATAL, "`%s' expects a context identifier",
                      pp_directives[i]);
                free_tlist(origline);
                return DIRECTIVE_FOUND;     /* but we did _something_ */
            }
            if (tline->next)
                nasm_error(ERR_WARNING|ERR_PASS1,
                      "trailing garbage after `%s' ignored",
                      pp_directives[i]);
            p = nasm_strdup(tline->text);
        } else {
            p = NULL; /* Anonymous */
        }

        if (i == PP_PUSH) {
            ctx = nasm_malloc(sizeof(Context));
            ctx->next = cstk;
            hash_init(&ctx->localmac, HASH_SMALL);
            ctx->name = p;
            ctx->number = unique++;
            cstk = ctx;
        } else {
            /* %pop or %repl */
            if (!cstk) {
                nasm_error(ERR_NONFATAL, "`%s': context stack is empty",
                      pp_directives[i]);
            } else if (i == PP_POP) {
                if (p && (!cstk->name || nasm_stricmp(p, cstk->name)))
                    nasm_error(ERR_NONFATAL, "`%%pop' in wrong context: %s, "
                          "expected %s",
                          cstk->name ? cstk->name : "anonymous", p);
                else
                    ctx_pop();
            } else {
                /* i == PP_REPL */
                nasm_free(cstk->name);
                cstk->name = p;
                p = NULL;
            }
            nasm_free(p);
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;
    case PP_FATAL:
        severity = ERR_FATAL;
        goto issue_error;
    case PP_ERROR:
        severity = ERR_NONFATAL;
        goto issue_error;
    case PP_WARNING:
        severity = ERR_WARNING|ERR_WARN_USER;
        goto issue_error;

issue_error:
    {
        /* Only error out if this is the final pass */
        if (pass != 2 && i != PP_FATAL)
            return DIRECTIVE_FOUND;

        tline->next = expand_smacro(tline->next);
        tline = tline->next;
        skip_white_(tline);
        t = tline ? tline->next : NULL;
        skip_white_(t);
        if (tok_type_(tline, TOK_STRING) && !t) {
            /* The line contains only a quoted string */
            p = tline->text;
            nasm_unquote(p, NULL); /* Ignore NUL character truncation */
            nasm_error(severity, "%s",  p);
        } else {
            /* Not a quoted string, or more than a quoted string */
            p = detoken(tline, false);
            nasm_error(severity, "%s",  p);
            nasm_free(p);
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;
    }

    CASE_PP_IF:
        if (istk->conds && !emitting(istk->conds->state))
            j = COND_NEVER;
        else {
            j = if_condition(tline->next, i);
            tline->next = NULL; /* it got freed */
            j = j < 0 ? COND_NEVER : j ? COND_IF_TRUE : COND_IF_FALSE;
        }
        cond = nasm_malloc(sizeof(Cond));
        cond->next = istk->conds;
        cond->state = j;
        istk->conds = cond;
        if(istk->mstk)
            istk->mstk->condcnt ++;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    CASE_PP_ELIF:
        if (!istk->conds)
            nasm_error(ERR_FATAL, "`%s': no matching `%%if'", pp_directives[i]);
        switch(istk->conds->state) {
        case COND_IF_TRUE:
            istk->conds->state = COND_DONE;
            break;

        case COND_DONE:
        case COND_NEVER:
            break;

        case COND_ELSE_TRUE:
        case COND_ELSE_FALSE:
	    nasm_error(ERR_WARNING|ERR_PASS1|ERR_PP_PRECOND,
		       "`%%elif' after `%%else' ignored");
            istk->conds->state = COND_NEVER;
            break;

        case COND_IF_FALSE:
            /*
             * IMPORTANT: In the case of %if, we will already have
             * called expand_mmac_params(); however, if we're
             * processing an %elif we must have been in a
             * non-emitting mode, which would have inhibited
             * the normal invocation of expand_mmac_params().
             * Therefore, we have to do it explicitly here.
             */
            j = if_condition(expand_mmac_params(tline->next), i);
            tline->next = NULL; /* it got freed */
            istk->conds->state =
                j < 0 ? COND_NEVER : j ? COND_IF_TRUE : COND_IF_FALSE;
            break;
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_ELSE:
        if (tline->next)
            nasm_error(ERR_WARNING|ERR_PASS1|ERR_PP_PRECOND,
		       "trailing garbage after `%%else' ignored");
        if (!istk->conds)
	    nasm_fatal(0, "`%%else: no matching `%%if'");
        switch(istk->conds->state) {
        case COND_IF_TRUE:
        case COND_DONE:
            istk->conds->state = COND_ELSE_FALSE;
            break;

        case COND_NEVER:
            break;

        case COND_IF_FALSE:
            istk->conds->state = COND_ELSE_TRUE;
            break;

        case COND_ELSE_TRUE:
        case COND_ELSE_FALSE:
            nasm_error(ERR_WARNING|ERR_PASS1|ERR_PP_PRECOND,
                          "`%%else' after `%%else' ignored.");
            istk->conds->state = COND_NEVER;
            break;
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_ENDIF:
        if (tline->next)
            nasm_error(ERR_WARNING|ERR_PASS1|ERR_PP_PRECOND,
		       "trailing garbage after `%%endif' ignored");
        if (!istk->conds)
            nasm_error(ERR_FATAL, "`%%endif': no matching `%%if'");
        cond = istk->conds;
        istk->conds = cond->next;
        nasm_free(cond);
        if(istk->mstk)
            istk->mstk->condcnt --;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_RMACRO:
    case PP_IRMACRO:
    case PP_MACRO:
    case PP_IMACRO:
        if (defining) {
            nasm_error(ERR_FATAL, "`%s': already defining a macro",
                  pp_directives[i]);
            return DIRECTIVE_FOUND;
        }
        defining = nasm_zalloc(sizeof(MMacro));
        defining->max_depth =
            (i == PP_RMACRO) || (i == PP_IRMACRO) ? DEADMAN_LIMIT : 0;
        defining->casesense = (i == PP_MACRO) || (i == PP_RMACRO);
        if (!parse_mmacro_spec(tline, defining, pp_directives[i])) {
            nasm_free(defining);
            defining = NULL;
            return DIRECTIVE_FOUND;
        }

	src_get(&defining->xline, &defining->fname);

        mmac = (MMacro *) hash_findix(&mmacros, defining->name);
        while (mmac) {
            if (!strcmp(mmac->name, defining->name) &&
                (mmac->nparam_min <= defining->nparam_max
                 || defining->plus)
                && (defining->nparam_min <= mmac->nparam_max
                    || mmac->plus)) {
                nasm_error(ERR_WARNING|ERR_PASS1,
                      "redefining multi-line macro `%s'", defining->name);
                return DIRECTIVE_FOUND;
            }
            mmac = mmac->next;
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_ENDM:
    case PP_ENDMACRO:
        if (! (defining && defining->name)) {
            nasm_error(ERR_NONFATAL, "`%s': not defining a macro", tline->text);
            return DIRECTIVE_FOUND;
        }
        mmhead = (MMacro **) hash_findi_add(&mmacros, defining->name);
        defining->next = *mmhead;
        *mmhead = defining;
        defining = NULL;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_EXITMACRO:
        /*
         * We must search along istk->expansion until we hit a
         * macro-end marker for a macro with a name. Then we
         * bypass all lines between exitmacro and endmacro.
         */
        list_for_each(l, istk->expansion)
            if (l->finishes && l->finishes->name)
                break;

        if (l) {
            /*
             * Remove all conditional entries relative to this
             * macro invocation. (safe to do in this context)
             */
            for ( ; l->finishes->condcnt > 0; l->finishes->condcnt --) {
                cond = istk->conds;
                istk->conds = cond->next;
                nasm_free(cond);
            }
            istk->expansion = l;
        } else {
            nasm_error(ERR_NONFATAL, "`%%exitmacro' not within `%%macro' block");
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_UNMACRO:
    case PP_UNIMACRO:
    {
        MMacro **mmac_p;
        MMacro spec;

        spec.casesense = (i == PP_UNMACRO);
        if (!parse_mmacro_spec(tline, &spec, pp_directives[i])) {
            return DIRECTIVE_FOUND;
        }
        mmac_p = (MMacro **) hash_findi(&mmacros, spec.name, NULL);
        while (mmac_p && *mmac_p) {
            mmac = *mmac_p;
            if (mmac->casesense == spec.casesense &&
                !mstrcmp(mmac->name, spec.name, spec.casesense) &&
                mmac->nparam_min == spec.nparam_min &&
                mmac->nparam_max == spec.nparam_max &&
                mmac->plus == spec.plus) {
                *mmac_p = mmac->next;
                free_mmacro(mmac);
            } else {
                mmac_p = &mmac->next;
            }
        }
        free_tlist(origline);
        free_tlist(spec.dlist);
        return DIRECTIVE_FOUND;
    }

    case PP_ROTATE:
        if (tline->next && tline->next->type == TOK_WHITESPACE)
            tline = tline->next;
        if (!tline->next) {
            free_tlist(origline);
            nasm_error(ERR_NONFATAL, "`%%rotate' missing rotate count");
            return DIRECTIVE_FOUND;
        }
        t = expand_smacro(tline->next);
        tline->next = NULL;
        free_tlist(origline);
        tline = t;
        tptr = &t;
        tokval.t_type = TOKEN_INVALID;
        evalresult =
            evaluate(ppscan, tptr, &tokval, NULL, pass, NULL);
        free_tlist(tline);
        if (!evalresult)
            return DIRECTIVE_FOUND;
        if (tokval.t_type)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after expression ignored");
        if (!is_simple(evalresult)) {
            nasm_error(ERR_NONFATAL, "non-constant value given to `%%rotate'");
            return DIRECTIVE_FOUND;
        }
        mmac = istk->mstk;
        while (mmac && !mmac->name)     /* avoid mistaking %reps for macros */
            mmac = mmac->next_active;
        if (!mmac) {
            nasm_error(ERR_NONFATAL, "`%%rotate' invoked outside a macro call");
        } else if (mmac->nparam == 0) {
            nasm_error(ERR_NONFATAL,
                  "`%%rotate' invoked within macro without parameters");
        } else {
            int rotate = mmac->rotate + reloc_value(evalresult);

            rotate %= (int)mmac->nparam;
            if (rotate < 0)
                rotate += mmac->nparam;

            mmac->rotate = rotate;
        }
        return DIRECTIVE_FOUND;

    case PP_REP:
        nolist = false;
        do {
            tline = tline->next;
        } while (tok_type_(tline, TOK_WHITESPACE));

        if (tok_type_(tline, TOK_ID) &&
            nasm_stricmp(tline->text, ".nolist") == 0) {
            nolist = true;
            do {
                tline = tline->next;
            } while (tok_type_(tline, TOK_WHITESPACE));
        }

        if (tline) {
            t = expand_smacro(tline);
            tptr = &t;
            tokval.t_type = TOKEN_INVALID;
            evalresult =
                evaluate(ppscan, tptr, &tokval, NULL, pass, NULL);
            if (!evalresult) {
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            if (tokval.t_type)
                nasm_error(ERR_WARNING|ERR_PASS1,
                      "trailing garbage after expression ignored");
            if (!is_simple(evalresult)) {
                nasm_error(ERR_NONFATAL, "non-constant value given to `%%rep'");
                return DIRECTIVE_FOUND;
            }
            count = reloc_value(evalresult);
            if (count >= REP_LIMIT) {
                nasm_error(ERR_NONFATAL, "`%%rep' value exceeds limit");
                count = 0;
            } else
                count++;
        } else {
            nasm_error(ERR_NONFATAL, "`%%rep' expects a repeat count");
            count = 0;
        }
        free_tlist(origline);

        tmp_defining = defining;
        defining = nasm_malloc(sizeof(MMacro));
        defining->prev = NULL;
        defining->name = NULL;  /* flags this macro as a %rep block */
        defining->casesense = false;
        defining->plus = false;
        defining->nolist = nolist;
        defining->in_progress = count;
        defining->max_depth = 0;
        defining->nparam_min = defining->nparam_max = 0;
        defining->defaults = NULL;
        defining->dlist = NULL;
        defining->expansion = NULL;
        defining->next_active = istk->mstk;
        defining->rep_nest = tmp_defining;
        return DIRECTIVE_FOUND;

    case PP_ENDREP:
        if (!defining || defining->name) {
            nasm_error(ERR_NONFATAL, "`%%endrep': no matching `%%rep'");
            return DIRECTIVE_FOUND;
        }

        /*
         * Now we have a "macro" defined - although it has no name
         * and we won't be entering it in the hash tables - we must
         * push a macro-end marker for it on to istk->expansion.
         * After that, it will take care of propagating itself (a
         * macro-end marker line for a macro which is really a %rep
         * block will cause the macro to be re-expanded, complete
         * with another macro-end marker to ensure the process
         * continues) until the whole expansion is forcibly removed
         * from istk->expansion by a %exitrep.
         */
        l = nasm_malloc(sizeof(Line));
        l->next = istk->expansion;
        l->finishes = defining;
        l->first = NULL;
        istk->expansion = l;

        istk->mstk = defining;

        lfmt->uplevel(defining->nolist ? LIST_MACRO_NOLIST : LIST_MACRO);
        tmp_defining = defining;
        defining = defining->rep_nest;
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_EXITREP:
        /*
         * We must search along istk->expansion until we hit a
         * macro-end marker for a macro with no name. Then we set
         * its `in_progress' flag to 0.
         */
        list_for_each(l, istk->expansion)
            if (l->finishes && !l->finishes->name)
                break;

        if (l)
            l->finishes->in_progress = 1;
        else
            nasm_error(ERR_NONFATAL, "`%%exitrep' not within `%%rep' block");
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_XDEFINE:
    case PP_IXDEFINE:
    case PP_DEFINE:
    case PP_IDEFINE:
        casesense = (i == PP_DEFINE || i == PP_XDEFINE);

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL, "`%s' expects a macro identifier",
                  pp_directives[i]);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        ctx = get_ctx(tline->text, &mname);
        last = tline;
        param_start = tline = tline->next;
        nparam = 0;

        /* Expand the macro definition now for %xdefine and %ixdefine */
        if ((i == PP_XDEFINE) || (i == PP_IXDEFINE))
            tline = expand_smacro(tline);

        if (tok_is_(tline, "(")) {
            /*
             * This macro has parameters.
             */

            tline = tline->next;
            while (1) {
                skip_white_(tline);
                if (!tline) {
                    nasm_error(ERR_NONFATAL, "parameter identifier expected");
                    free_tlist(origline);
                    return DIRECTIVE_FOUND;
                }
                if (tline->type != TOK_ID) {
                    nasm_error(ERR_NONFATAL,
                          "`%s': parameter identifier expected",
                          tline->text);
                    free_tlist(origline);
                    return DIRECTIVE_FOUND;
                }
                tline->type = TOK_SMAC_PARAM + nparam++;
                tline = tline->next;
                skip_white_(tline);
                if (tok_is_(tline, ",")) {
                    tline = tline->next;
                } else {
                    if (!tok_is_(tline, ")")) {
                        nasm_error(ERR_NONFATAL,
                              "`)' expected to terminate macro template");
                        free_tlist(origline);
                        return DIRECTIVE_FOUND;
                    }
                    break;
                }
            }
            last = tline;
            tline = tline->next;
        }
        if (tok_type_(tline, TOK_WHITESPACE))
            last = tline, tline = tline->next;
        macro_start = NULL;
        last->next = NULL;
        t = tline;
        while (t) {
            if (t->type == TOK_ID) {
                list_for_each(tt, param_start)
                    if (tt->type >= TOK_SMAC_PARAM &&
                        !strcmp(tt->text, t->text))
                        t->type = tt->type;
            }
            tt = t->next;
            t->next = macro_start;
            macro_start = t;
            t = tt;
        }
        /*
         * Good. We now have a macro name, a parameter count, and a
         * token list (in reverse order) for an expansion. We ought
         * to be OK just to create an SMacro, store it, and let
         * free_tlist have the rest of the line (which we have
         * carefully re-terminated after chopping off the expansion
         * from the end).
         */
        define_smacro(ctx, mname, casesense, nparam, macro_start);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_UNDEF:
        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL, "`%%undef' expects a macro identifier");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        if (tline->next) {
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after macro name ignored");
        }

        /* Find the context that symbol belongs to */
        ctx = get_ctx(tline->text, &mname);
        undef_smacro(ctx, mname);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_DEFSTR:
    case PP_IDEFSTR:
        casesense = (i == PP_DEFSTR);

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL, "`%s' expects a macro identifier",
                  pp_directives[i]);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        while (tok_type_(tline, TOK_WHITESPACE))
            tline = delete_Token(tline);

        p = detoken(tline, false);
        macro_start = nasm_malloc(sizeof(*macro_start));
        macro_start->next = NULL;
        macro_start->text = nasm_quote(p, strlen(p));
        macro_start->type = TOK_STRING;
        macro_start->a.mac = NULL;
        nasm_free(p);

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a string token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_DEFTOK:
    case PP_IDEFTOK:
        casesense = (i == PP_DEFTOK);

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%s' expects a macro identifier as first parameter",
                  pp_directives[i]);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        t = tline;
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;
        /* t should now point to the string */
        if (!tok_type_(t, TOK_STRING)) {
            nasm_error(ERR_NONFATAL,
                  "`%s` requires string as second parameter",
                  pp_directives[i]);
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        /*
         * Convert the string to a token stream.  Note that smacros
         * are stored with the token stream reversed, so we have to
         * reverse the output of tokenize().
         */
        nasm_unquote_cstr(t->text, i);
        macro_start = reverse_tokens(tokenize(t->text));

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a numeric token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(tline);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_PATHSEARCH:
    {
        const char *found_path;

        casesense = true;

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%%pathsearch' expects a macro identifier as first parameter");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        t = tline;
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;

        if (!t || (t->type != TOK_STRING &&
                   t->type != TOK_INTERNAL_STRING)) {
            nasm_error(ERR_NONFATAL, "`%%pathsearch' expects a file name");
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;     /* but we did _something_ */
        }
        if (t->next)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after `%%pathsearch' ignored");
        p = t->text;
        if (t->type != TOK_INTERNAL_STRING)
            nasm_unquote(p, NULL);

        inc_fopen(p, NULL, &found_path, INC_PROBE, NF_BINARY);
        if (!found_path)
            found_path = p;
        macro_start = nasm_malloc(sizeof(*macro_start));
        macro_start->next = NULL;
        macro_start->text = nasm_quote(found_path, strlen(found_path));
        macro_start->type = TOK_STRING;
        macro_start->a.mac = NULL;

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a string token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(tline);
        free_tlist(origline);
        return DIRECTIVE_FOUND;
    }

    case PP_STRLEN:
        casesense = true;

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%%strlen' expects a macro identifier as first parameter");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        t = tline;
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;
        /* t should now point to the string */
        if (!tok_type_(t, TOK_STRING)) {
            nasm_error(ERR_NONFATAL,
                  "`%%strlen` requires string as second parameter");
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        macro_start = nasm_malloc(sizeof(*macro_start));
        macro_start->next = NULL;
        make_tok_num(macro_start, nasm_unquote(t->text, NULL));
        macro_start->a.mac = NULL;

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a numeric token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(tline);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_STRCAT:
        casesense = true;

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%%strcat' expects a macro identifier as first parameter");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        len = 0;
        list_for_each(t, tline) {
            switch (t->type) {
            case TOK_WHITESPACE:
                break;
            case TOK_STRING:
                len += t->a.len = nasm_unquote(t->text, NULL);
                break;
            case TOK_OTHER:
                if (!strcmp(t->text, ",")) /* permit comma separators */
                    break;
                /* else fall through */
            default:
                nasm_error(ERR_NONFATAL,
                      "non-string passed to `%%strcat' (%d)", t->type);
                free_tlist(tline);
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
        }

        p = pp = nasm_malloc(len);
        list_for_each(t, tline) {
            if (t->type == TOK_STRING) {
                memcpy(p, t->text, t->a.len);
                p += t->a.len;
            }
        }

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a numeric token to use as an expansion. Create
         * and store an SMacro.
         */
        macro_start = new_Token(NULL, TOK_STRING, NULL, 0);
        macro_start->text = nasm_quote(pp, len);
        nasm_free(pp);
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(tline);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_SUBSTR:
    {
        int64_t start, count;
        size_t len;

        casesense = true;

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%%substr' expects a macro identifier as first parameter");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        if (tline) /* skip expanded id */
            t = tline->next;
        while (tok_type_(t, TOK_WHITESPACE))
            t = t->next;

        /* t should now point to the string */
        if (!tok_type_(t, TOK_STRING)) {
            nasm_error(ERR_NONFATAL,
                  "`%%substr` requires string as second parameter");
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        tt = t->next;
        tptr = &tt;
        tokval.t_type = TOKEN_INVALID;
        evalresult = evaluate(ppscan, tptr, &tokval, NULL, pass, NULL);
        if (!evalresult) {
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        } else if (!is_simple(evalresult)) {
            nasm_error(ERR_NONFATAL, "non-constant value given to `%%substr`");
            free_tlist(tline);
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        start = evalresult->value - 1;

        while (tok_type_(tt, TOK_WHITESPACE))
            tt = tt->next;
        if (!tt) {
            count = 1;  /* Backwards compatibility: one character */
        } else {
            tokval.t_type = TOKEN_INVALID;
            evalresult = evaluate(ppscan, tptr, &tokval, NULL, pass, NULL);
            if (!evalresult) {
                free_tlist(tline);
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            } else if (!is_simple(evalresult)) {
                nasm_error(ERR_NONFATAL, "non-constant value given to `%%substr`");
                free_tlist(tline);
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            count = evalresult->value;
        }

        len = nasm_unquote(t->text, NULL);

        /* make start and count being in range */
        if (start < 0)
            start = 0;
        if (count < 0)
            count = len + count + 1 - start;
        if (start + count > (int64_t)len)
            count = len - start;
        if (!len || count < 0 || start >=(int64_t)len)
            start = -1, count = 0; /* empty string */

        macro_start = nasm_malloc(sizeof(*macro_start));
        macro_start->next = NULL;
        macro_start->text = nasm_quote((start < 0) ? "" : t->text + start, count);
        macro_start->type = TOK_STRING;
        macro_start->a.mac = NULL;

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a numeric token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(tline);
        free_tlist(origline);
        return DIRECTIVE_FOUND;
    }

    case PP_ASSIGN:
    case PP_IASSIGN:
        casesense = (i == PP_ASSIGN);

        tline = tline->next;
        skip_white_(tline);
        tline = expand_id(tline);
        if (!tline || (tline->type != TOK_ID &&
                       (tline->type != TOK_PREPROC_ID ||
                        tline->text[1] != '$'))) {
            nasm_error(ERR_NONFATAL,
                  "`%%%sassign' expects a macro identifier",
                  (i == PP_IASSIGN ? "i" : ""));
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        ctx = get_ctx(tline->text, &mname);
        last = tline;
        tline = expand_smacro(tline->next);
        last->next = NULL;

        t = tline;
        tptr = &t;
        tokval.t_type = TOKEN_INVALID;
        evalresult = evaluate(ppscan, tptr, &tokval, NULL, pass, NULL);
        free_tlist(tline);
        if (!evalresult) {
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        if (tokval.t_type)
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "trailing garbage after expression ignored");

        if (!is_simple(evalresult)) {
            nasm_error(ERR_NONFATAL,
                  "non-constant value given to `%%%sassign'",
                  (i == PP_IASSIGN ? "i" : ""));
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }

        macro_start = nasm_malloc(sizeof(*macro_start));
        macro_start->next = NULL;
        make_tok_num(macro_start, reloc_value(evalresult));
        macro_start->a.mac = NULL;

        /*
         * We now have a macro name, an implicit parameter count of
         * zero, and a numeric token to use as an expansion. Create
         * and store an SMacro.
         */
        define_smacro(ctx, mname, casesense, 0, macro_start);
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    case PP_LINE:
        /*
         * Syntax is `%line nnn[+mmm] [filename]'
         */
        tline = tline->next;
        skip_white_(tline);
        if (!tok_type_(tline, TOK_NUMBER)) {
            nasm_error(ERR_NONFATAL, "`%%line' expects line number");
            free_tlist(origline);
            return DIRECTIVE_FOUND;
        }
        k = readnum(tline->text, &err);
        m = 1;
        tline = tline->next;
        if (tok_is_(tline, "+")) {
            tline = tline->next;
            if (!tok_type_(tline, TOK_NUMBER)) {
                nasm_error(ERR_NONFATAL, "`%%line' expects line increment");
                free_tlist(origline);
                return DIRECTIVE_FOUND;
            }
            m = readnum(tline->text, &err);
            tline = tline->next;
        }
        skip_white_(tline);
        src_set_linnum(k);
        istk->lineinc = m;
        if (tline) {
            char *fname = detoken(tline, false);
            src_set_fname(fname);
            nasm_free(fname);
        }
        free_tlist(origline);
        return DIRECTIVE_FOUND;

    default:
        nasm_error(ERR_FATAL,
              "preprocessor directive `%s' not yet implemented",
              pp_directives[i]);
        return DIRECTIVE_FOUND;
    }
}