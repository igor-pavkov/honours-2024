static void undef_smacro(Context *ctx, const char *mname)
{
    SMacro **smhead, *s, **sp;
    struct hash_table *smtbl;

    smtbl = ctx ? &ctx->localmac : &smacros;
    smhead = (SMacro **)hash_findi(smtbl, mname, NULL);

    if (smhead) {
        /*
         * We now have a macro name... go hunt for it.
         */
        sp = smhead;
        while ((s = *sp) != NULL) {
            if (!mstrcmp(s->name, mname, s->casesense)) {
                *sp = s->next;
                nasm_free(s->name);
                free_tlist(s->expansion);
                nasm_free(s);
            } else {
                sp = &s->next;
            }
        }
    }
}