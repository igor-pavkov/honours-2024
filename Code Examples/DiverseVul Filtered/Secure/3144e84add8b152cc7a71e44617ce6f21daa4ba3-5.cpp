static bool parse_mmacro_spec(Token *tline, MMacro *def, const char *directive)
{
    bool err;

    tline = tline->next;
    skip_white_(tline);
    tline = expand_id(tline);
    if (!tok_type_(tline, TOK_ID)) {
        nasm_error(ERR_NONFATAL, "`%s' expects a macro name", directive);
        return false;
    }

    def->prev = NULL;
    def->name = nasm_strdup(tline->text);
    def->plus = false;
    def->nolist = false;
    def->in_progress = 0;
    def->rep_nest = NULL;
    def->nparam_min = 0;
    def->nparam_max = 0;

    tline = expand_smacro(tline->next);
    skip_white_(tline);
    if (!tok_type_(tline, TOK_NUMBER)) {
        nasm_error(ERR_NONFATAL, "`%s' expects a parameter count", directive);
    } else {
        def->nparam_min = def->nparam_max =
            readnum(tline->text, &err);
        if (err)
            nasm_error(ERR_NONFATAL,
                  "unable to parse parameter count `%s'", tline->text);
    }
    if (tline && tok_is_(tline->next, "-")) {
        tline = tline->next->next;
        if (tok_is_(tline, "*")) {
            def->nparam_max = INT_MAX;
        } else if (!tok_type_(tline, TOK_NUMBER)) {
            nasm_error(ERR_NONFATAL,
                  "`%s' expects a parameter count after `-'", directive);
        } else {
            def->nparam_max = readnum(tline->text, &err);
            if (err) {
                nasm_error(ERR_NONFATAL, "unable to parse parameter count `%s'",
                      tline->text);
            }
            if (def->nparam_min > def->nparam_max) {
                nasm_error(ERR_NONFATAL, "minimum parameter count exceeds maximum");
            }
        }
    }
    if (tline && tok_is_(tline->next, "+")) {
        tline = tline->next;
        def->plus = true;
    }
    if (tline && tok_type_(tline->next, TOK_ID) &&
        !nasm_stricmp(tline->next->text, ".nolist")) {
        tline = tline->next;
        def->nolist = true;
    }

    /*
     * Handle default parameters.
     */
    if (tline && tline->next) {
        def->dlist = tline->next;
        tline->next = NULL;
        count_mmac_params(def->dlist, &def->ndefs, &def->defaults);
    } else {
        def->dlist = NULL;
        def->defaults = NULL;
    }
    def->expansion = NULL;

    if (def->defaults && def->ndefs > def->nparam_max - def->nparam_min &&
        !def->plus)
        nasm_error(ERR_WARNING|ERR_PASS1|ERR_WARN_MDP,
              "too many default macro parameters");

    return true;
}