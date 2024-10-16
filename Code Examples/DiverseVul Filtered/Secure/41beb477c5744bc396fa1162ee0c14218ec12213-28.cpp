int UZ_EXP UzpPassword (pG, rcnt, pwbuf, size, zfn, efn)
    zvoid *pG;         /* pointer to UnZip's internal global vars */
    int *rcnt;         /* retry counter */
    char *pwbuf;       /* buffer for password */
    int size;          /* size of password buffer */
    ZCONST char *zfn;  /* name of zip archive */
    ZCONST char *efn;  /* name of archive entry being processed */
{
#if CRYPT
    int r = IZ_PW_ENTERED;
    char *m;
    char *prompt;

#ifndef REENTRANT
    /* tell picky compilers to shut up about "unused variable" warnings */
    (void)pG;
#endif

    if (*rcnt == 0) {           /* First call for current entry */
        *rcnt = 2;
        if ((prompt = (char *)malloc(2*FILNAMSIZ + 15)) != (char *)NULL) {
            sprintf(prompt, LoadFarString(PasswPrompt),
                    FnFilter1(zfn), FnFilter2(efn));
            m = prompt;
        } else
            m = (char *)LoadFarString(PasswPrompt2);
    } else {                    /* Retry call, previous password was wrong */
        (*rcnt)--;
        prompt = NULL;
        m = (char *)LoadFarString(PasswRetry);
    }

    m = getp(__G__ m, pwbuf, size);
    if (prompt != (char *)NULL) {
        free(prompt);
    }
    if (m == (char *)NULL) {
        r = IZ_PW_ERROR;
    }
    else if (*pwbuf == '\0') {
        r = IZ_PW_CANCELALL;
    }
    return r;

#else /* !CRYPT */
    /* tell picky compilers to shut up about "unused variable" warnings */
    (void)pG; (void)rcnt; (void)pwbuf; (void)size; (void)zfn; (void)efn;

    return IZ_PW_ERROR;  /* internal error; function should never get called */
#endif /* ?CRYPT */

} /* end function UzpPassword() */