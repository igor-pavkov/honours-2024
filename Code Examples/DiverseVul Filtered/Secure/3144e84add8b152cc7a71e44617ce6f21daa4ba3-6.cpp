static MMacro *is_mmacro(Token * tline, Token *** params_array)
{
    MMacro *head, *m;
    Token **params;
    int nparam;

    head = (MMacro *) hash_findix(&mmacros, tline->text);

    /*
     * Efficiency: first we see if any macro exists with the given
     * name. If not, we can return NULL immediately. _Then_ we
     * count the parameters, and then we look further along the
     * list if necessary to find the proper MMacro.
     */
    list_for_each(m, head)
        if (!mstrcmp(m->name, tline->text, m->casesense))
            break;
    if (!m)
        return NULL;

    /*
     * OK, we have a potential macro. Count and demarcate the
     * parameters.
     */
    count_mmac_params(tline->next, &nparam, &params);

    /*
     * So we know how many parameters we've got. Find the MMacro
     * structure that handles this number.
     */
    while (m) {
        if (m->nparam_min <= nparam
            && (m->plus || nparam <= m->nparam_max)) {
            /*
             * This one is right. Just check if cycle removal
             * prohibits us using it before we actually celebrate...
             */
            if (m->in_progress > m->max_depth) {
                if (m->max_depth > 0) {
                    nasm_error(ERR_WARNING,
                          "reached maximum recursion depth of %i",
                          m->max_depth);
                }
                nasm_free(params);
                return NULL;
            }
            /*
             * It's right, and we can use it. Add its default
             * parameters to the end of our list if necessary.
             */
            if (m->defaults && nparam < m->nparam_min + m->ndefs) {
                params =
                    nasm_realloc(params,
                                 ((m->nparam_min + m->ndefs +
                                   1) * sizeof(*params)));
                while (nparam < m->nparam_min + m->ndefs) {
                    params[nparam] = m->defaults[nparam - m->nparam_min];
                    nparam++;
                }
            }
            /*
             * If we've gone over the maximum parameter count (and
             * we're in Plus mode), ignore parameters beyond
             * nparam_max.
             */
            if (m->plus && nparam > m->nparam_max)
                nparam = m->nparam_max;
            /*
             * Then terminate the parameter list, and leave.
             */
            if (!params) {      /* need this special case */
                params = nasm_malloc(sizeof(*params));
                nparam = 0;
            }
            params[nparam] = NULL;
            *params_array = params;
            return m;
        }
        /*
         * This one wasn't right: look for the next one with the
         * same name.
         */
        list_for_each(m, m->next)
            if (!mstrcmp(m->name, tline->text, m->casesense))
                break;
    }

    /*
     * After all that, we didn't find one with the right number of
     * parameters. Issue a warning, and fail to expand the macro.
     */
    nasm_error(ERR_WARNING|ERR_PASS1|ERR_WARN_MNP,
          "macro `%s' exists, but not taking %d parameters",
          tline->text, nparam);
    nasm_free(params);
    return NULL;
}