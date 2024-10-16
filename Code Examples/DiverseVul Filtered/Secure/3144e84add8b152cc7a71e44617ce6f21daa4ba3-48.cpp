static void pp_error_list_macros(int severity)
{
    int32_t saved_line;
    const char *saved_fname = NULL;

    severity |= ERR_PP_LISTMACRO | ERR_NO_SEVERITY;
    src_get(&saved_line, &saved_fname);

    if (istk)
        pp_list_one_macro(istk->mstk, severity);

    src_set(saved_line, saved_fname);
}