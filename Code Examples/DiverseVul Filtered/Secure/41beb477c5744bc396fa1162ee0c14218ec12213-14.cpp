extent uzmbclen(ptr)
    ZCONST unsigned char *ptr;
{
    int mbl;

    mbl = mblen((ZCONST char *)ptr, MB_CUR_MAX);
    /* For use in code scanning through MBCS strings, we need a strictly
       positive "MB char bytes count".  For our scanning purpose, it is not
       not relevant whether the MB character is valid or not. And, the NUL
       char '\0' has a byte count of 1, but mblen() returns 0. So, we make
       sure that the uzmbclen() return value is not less than 1.
     */
    return (extent)(mbl > 0 ? mbl : 1);
}