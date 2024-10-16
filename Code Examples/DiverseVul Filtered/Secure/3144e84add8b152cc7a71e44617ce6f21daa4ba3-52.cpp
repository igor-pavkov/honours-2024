static void do_pragma_preproc(Token *tline)
{
    /* Skip to the real stuff */
    tline = tline->next;
    skip_white_(tline);
    if (!tline)
        return;

    (void)tline;                /* Nothing else to do at present */
}