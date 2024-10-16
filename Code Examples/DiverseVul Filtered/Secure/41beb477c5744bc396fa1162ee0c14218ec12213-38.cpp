char *fzofft(__G__ val, pre, post)
    __GDEF
    zoff_t val;
    ZCONST char *pre;
    ZCONST char *post;
{
    /* Storage cylinder. (now in globals.h) */
    /*static char fzofft_buf[FZOFFT_NUM][FZOFFT_LEN];*/
    /*static int fzofft_index = 0;*/

    /* Temporary format string storage. */
    char fmt[16];

    /* Assemble the format string. */
    fmt[0] = '%';
    fmt[1] = '\0';             /* Start after initial "%". */
    if (pre == FZOFFT_HEX_WID)  /* Special hex width. */
    {
        strcat(fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre == FZOFFT_HEX_DOT_WID) /* Special hex ".width". */
    {
        strcat(fmt, ".");
        strcat(fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre != NULL)       /* Caller's prefix (width). */
    {
        strcat(fmt, pre);
    }

    strcat(fmt, FZOFFT_FMT);   /* Long or long-long or whatever. */

    if (post == NULL)
        strcat(fmt, "d");      /* Default radix = decimal. */
    else
        strcat(fmt, post);     /* Caller's radix. */

    /* Advance the cylinder. */
    G.fzofft_index = (G.fzofft_index + 1) % FZOFFT_NUM;

    /* Write into the current chamber. */
    sprintf(G.fzofft_buf[G.fzofft_index], fmt, val);

    /* Return a pointer to this chamber. */
    return G.fzofft_buf[G.fzofft_index];
}