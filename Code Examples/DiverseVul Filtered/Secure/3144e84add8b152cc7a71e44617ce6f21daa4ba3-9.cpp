static void pp_verror(int severity, const char *fmt, va_list arg)
{
    char buff[BUFSIZ];
    MMacro *mmac = NULL;
    int delta = 0;

    /*
     * If we're in a dead branch of IF or something like it, ignore the error.
     * However, because %else etc are evaluated in the state context
     * of the previous branch, errors might get lost:
     *   %if 0 ... %else trailing garbage ... %endif
     * So %else etc should set the ERR_PP_PRECOND flag.
     */
    if ((severity & ERR_MASK) < ERR_FATAL &&
	istk && istk->conds &&
	((severity & ERR_PP_PRECOND) ?
	 istk->conds->state == COND_NEVER :
	 !emitting(istk->conds->state)))
	return;

    /* get %macro name */
    if (!(severity & ERR_NOFILE) && istk && istk->mstk) {
        mmac = istk->mstk;
        /* but %rep blocks should be skipped */
        while (mmac && !mmac->name)
            mmac = mmac->next_active, delta++;
    }

    if (mmac) {
	vsnprintf(buff, sizeof(buff), fmt, arg);

	nasm_set_verror(real_verror);
	nasm_error(severity, "(%s:%d) %s",
		   mmac->name, mmac->lineno - delta, buff);
	nasm_set_verror(pp_verror);
    } else {
        real_verror(severity, fmt, arg);
    }
}