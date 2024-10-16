static char *pp_getline(void)
{
    char *line;
    Token *tline;

    real_verror = nasm_set_verror(pp_verror);

    while (1) {
        /*
         * Fetch a tokenized line, either from the macro-expansion
         * buffer or from the input file.
         */
        tline = NULL;
        while (istk->expansion && istk->expansion->finishes) {
            Line *l = istk->expansion;
            if (!l->finishes->name && l->finishes->in_progress > 1) {
                Line *ll;

                /*
                 * This is a macro-end marker for a macro with no
                 * name, which means it's not really a macro at all
                 * but a %rep block, and the `in_progress' field is
                 * more than 1, meaning that we still need to
                 * repeat. (1 means the natural last repetition; 0
                 * means termination by %exitrep.) We have
                 * therefore expanded up to the %endrep, and must
                 * push the whole block on to the expansion buffer
                 * again. We don't bother to remove the macro-end
                 * marker: we'd only have to generate another one
                 * if we did.
                 */
                l->finishes->in_progress--;
                list_for_each(l, l->finishes->expansion) {
                    Token *t, *tt, **tail;

                    ll = nasm_malloc(sizeof(Line));
                    ll->next = istk->expansion;
                    ll->finishes = NULL;
                    ll->first = NULL;
                    tail = &ll->first;

                    list_for_each(t, l->first) {
                        if (t->text || t->type == TOK_WHITESPACE) {
                            tt = *tail = new_Token(NULL, t->type, t->text, 0);
                            tail = &tt->next;
                        }
                    }

                    istk->expansion = ll;
                }
            } else {
                /*
                 * Check whether a `%rep' was started and not ended
                 * within this macro expansion. This can happen and
                 * should be detected. It's a fatal error because
                 * I'm too confused to work out how to recover
                 * sensibly from it.
                 */
                if (defining) {
                    if (defining->name)
                        nasm_panic(0, "defining with name in expansion");
                    else if (istk->mstk->name)
                        nasm_fatal(0, "`%%rep' without `%%endrep' within"
				   " expansion of macro `%s'",
				   istk->mstk->name);
                }

                /*
                 * FIXME:  investigate the relationship at this point between
                 * istk->mstk and l->finishes
                 */
                {
                    MMacro *m = istk->mstk;
                    istk->mstk = m->next_active;
                    if (m->name) {
                        /*
                         * This was a real macro call, not a %rep, and
                         * therefore the parameter information needs to
                         * be freed.
                         */
                        if (m->prev) {
                            pop_mmacro(m);
                            l->finishes->in_progress --;
                        } else {
                            nasm_free(m->params);
                            free_tlist(m->iline);
                            nasm_free(m->paramlen);
                            l->finishes->in_progress = 0;
                        }
                    }

                    /*
                     * FIXME It is incorrect to always free_mmacro here.
                     * It leads to usage-after-free.
                     *
                     * https://bugzilla.nasm.us/show_bug.cgi?id=3392414
                     */
#if 0
                    else
                        free_mmacro(m);
#endif
                }
                istk->expansion = l->next;
                nasm_free(l);
                lfmt->downlevel(LIST_MACRO);
            }
        }
        while (1) {             /* until we get a line we can use */

            if (istk->expansion) {      /* from a macro expansion */
                char *p;
                Line *l = istk->expansion;
                if (istk->mstk)
                    istk->mstk->lineno++;
                tline = l->first;
                istk->expansion = l->next;
                nasm_free(l);
                p = detoken(tline, false);
                lfmt->line(LIST_MACRO, p);
                nasm_free(p);
                break;
            }
            line = read_line();
            if (line) {         /* from the current input file */
                line = prepreproc(line);
                tline = tokenize(line);
                nasm_free(line);
                break;
            }
            /*
             * The current file has ended; work down the istk
             */
            {
                Include *i = istk;
                fclose(i->fp);
                if (i->conds) {
                    /* nasm_error can't be conditionally suppressed */
                    nasm_fatal(0,
                               "expected `%%endif' before end of file");
                }
                /* only set line and file name if there's a next node */
                if (i->next)
                    src_set(i->lineno, i->fname);
                istk = i->next;
                lfmt->downlevel(LIST_INCLUDE);
                nasm_free(i);
                if (!istk) {
		    line = NULL;
		    goto done;
		}
                if (istk->expansion && istk->expansion->finishes)
                    break;
            }
        }

        /*
         * We must expand MMacro parameters and MMacro-local labels
         * _before_ we plunge into directive processing, to cope
         * with things like `%define something %1' such as STRUC
         * uses. Unless we're _defining_ a MMacro, in which case
         * those tokens should be left alone to go into the
         * definition; and unless we're in a non-emitting
         * condition, in which case we don't want to meddle with
         * anything.
         */
        if (!defining && !(istk->conds && !emitting(istk->conds->state))
            && !(istk->mstk && !istk->mstk->in_progress)) {
            tline = expand_mmac_params(tline);
        }

        /*
         * Check the line to see if it's a preprocessor directive.
         */
        if (do_directive(tline, &line) == DIRECTIVE_FOUND) {
            if (line)
                break;          /* Directive generated output */
            else
                continue;
        } else if (defining) {
            /*
             * We're defining a multi-line macro. We emit nothing
             * at all, and just
             * shove the tokenized line on to the macro definition.
             */
            Line *l = nasm_malloc(sizeof(Line));
            l->next = defining->expansion;
            l->first = tline;
            l->finishes = NULL;
            defining->expansion = l;
            continue;
        } else if (istk->conds && !emitting(istk->conds->state)) {
            /*
             * We're in a non-emitting branch of a condition block.
             * Emit nothing at all, not even a blank line: when we
             * emerge from the condition we'll give a line-number
             * directive so we keep our place correctly.
             */
            free_tlist(tline);
            continue;
        } else if (istk->mstk && !istk->mstk->in_progress) {
            /*
             * We're in a %rep block which has been terminated, so
             * we're walking through to the %endrep without
             * emitting anything. Emit nothing at all, not even a
             * blank line: when we emerge from the %rep block we'll
             * give a line-number directive so we keep our place
             * correctly.
             */
            free_tlist(tline);
            continue;
        } else {
            tline = expand_smacro(tline);
            if (!expand_mmacro(tline)) {
                /*
                 * De-tokenize the line again, and emit it.
                 */
                line = detoken(tline, true);
                free_tlist(tline);
                break;
            } else {
                continue;       /* expand_mmacro calls free_tlist */
            }
        }
    }

done:
    nasm_set_verror(real_verror);
    return line;
}