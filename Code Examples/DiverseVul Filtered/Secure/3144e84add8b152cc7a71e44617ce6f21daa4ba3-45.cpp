static bool define_smacro(Context *ctx, const char *mname, bool casesense,
                          int nparam, Token *expansion)
{
    SMacro *smac, **smhead;
    struct hash_table *smtbl;

    if (smacro_defined(ctx, mname, nparam, &smac, casesense)) {
        if (!smac) {
            nasm_error(ERR_WARNING|ERR_PASS1,
                  "single-line macro `%s' defined both with and"
                  " without parameters", mname);
            /*
             * Some instances of the old code considered this a failure,
             * some others didn't.  What is the right thing to do here?
             */
            free_tlist(expansion);
            return false;       /* Failure */
        } else {
            /*
             * We're redefining, so we have to take over an
             * existing SMacro structure. This means freeing
             * what was already in it.
             */
            nasm_free(smac->name);
            free_tlist(smac->expansion);
        }
    } else {
        smtbl  = ctx ? &ctx->localmac : &smacros;
        smhead = (SMacro **) hash_findi_add(smtbl, mname);
        smac = nasm_malloc(sizeof(SMacro));
        smac->next = *smhead;
        *smhead = smac;
    }
    smac->name = nasm_strdup(mname);
    smac->casesense = casesense;
    smac->nparam = nparam;
    smac->expansion = expansion;
    smac->in_progress = false;
    return true;                /* Success */
}