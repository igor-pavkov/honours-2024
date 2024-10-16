static int is_condition(enum preproc_token arg)
{
    return PP_IS_COND(arg) || (arg == PP_ELSE) || (arg == PP_ENDIF);
}