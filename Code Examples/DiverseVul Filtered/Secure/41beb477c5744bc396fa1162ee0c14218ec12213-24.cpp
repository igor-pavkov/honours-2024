void UZ_EXP UzpMorePause(pG, prompt, flag)
    zvoid *pG;            /* globals struct:  always passed */
    ZCONST char *prompt;  /* "--More--" prompt */
    int flag;             /* 0 = any char OK; 1 = accept only '\n', ' ', q */
{
    uch c;

/*---------------------------------------------------------------------------
    Print a prompt and wait for the user to press a key, then erase prompt
    if possible.
  ---------------------------------------------------------------------------*/

    if (!((Uz_Globs *)pG)->sol)
        fprintf(stderr, "\n");
    /* numlines may or may not be used: */
    fprintf(stderr, prompt, ((Uz_Globs *)pG)->numlines);
    fflush(stderr);
    if (flag & 1) {
        do {
            c = (uch)FGETCH(0);
        } while (
#ifdef THEOS
                 c != 17 &&     /* standard QUIT key */
#endif
                 c != '\r' && c != '\n' && c != ' ' && c != 'q' && c != 'Q');
    } else
        c = (uch)FGETCH(0);

    /* newline was not echoed, so cover up prompt line */
    fprintf(stderr, LoadFarString(HidePrompt));
    fflush(stderr);

    if (
#ifdef THEOS
        (c == 17) ||            /* standard QUIT key */
#endif
        (ToLower(c) == 'q')) {
        DESTROYGLOBALS();
        EXIT(PK_COOL);
    }

    ((Uz_Globs *)pG)->sol = TRUE;

#ifdef MORE
    /* space for another screen, enter for another line. */
    if ((flag & 1) && c == ' ')
        ((Uz_Globs *)pG)->lines = 0;
#endif /* MORE */

} /* end function UzpMorePause() */