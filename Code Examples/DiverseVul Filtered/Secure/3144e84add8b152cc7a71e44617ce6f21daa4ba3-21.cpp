static void pp_cleanup(int pass)
{
    real_verror = nasm_set_verror(pp_verror);

    if (defining) {
        if (defining->name) {
            nasm_error(ERR_NONFATAL,
		       "end of file while still defining macro `%s'",
		       defining->name);
        } else {
            nasm_error(ERR_NONFATAL, "end of file while still in %%rep");
        }

        free_mmacro(defining);
        defining = NULL;
    }

    nasm_set_verror(real_verror);

    while (cstk)
        ctx_pop();
    free_macros();
    while (istk) {
        Include *i = istk;
        istk = istk->next;
        fclose(i->fp);
        nasm_free(i);
    }
    while (cstk)
        ctx_pop();
    src_set_fname(NULL);
    if (pass == 0) {
        IncPath *i;
        free_llist(predef);
        predef = NULL;
        delete_Blocks();
        freeTokens = NULL;
        while ((i = ipath)) {
            ipath = i->next;
            if (i->path)
                nasm_free(i->path);
            nasm_free(i);
        }
    }
}