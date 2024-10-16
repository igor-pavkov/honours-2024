int UZ_EXP UzpInput(pG, buf, size, flag)
    zvoid *pG;    /* globals struct:  always passed */
    uch *buf;     /* preformatted string to be printed */
    int *size;    /* (address of) size of buf and of returned string */
    int flag;     /* flag bits (bit 0: no echo) */
{
    /* tell picky compilers to shut up about "unused variable" warnings */
    (void)pG; (void)buf; (void)flag;

    *size = 0;
    return 0;

} /* end function UzpInput() */