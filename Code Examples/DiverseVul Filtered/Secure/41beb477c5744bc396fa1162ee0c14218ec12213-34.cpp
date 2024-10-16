zusz_t makeint64(sig)
    ZCONST uch *sig;
{
#ifdef LARGE_FILE_SUPPORT
    /*
     * Convert intel style 'int64' variable to non-Intel non-16-bit
     * host format.  This routine also takes care of byte-ordering.
     */
    return (((zusz_t)sig[7]) << 56)
        + (((zusz_t)sig[6]) << 48)
        + (((zusz_t)sig[4]) << 32)
        + (zusz_t)((((ulg)sig[3]) << 24)
                 + (((ulg)sig[2]) << 16)
                 + (((unsigned)sig[1]) << 8)
                 + (sig[0]));

#else /* !LARGE_FILE_SUPPORT */

    if ((sig[7] | sig[6] | sig[5] | sig[4]) != 0)
        return (zusz_t)0xffffffffL;
    else
        return (zusz_t)((((ulg)sig[3]) << 24)
                      + (((ulg)sig[2]) << 16)
                      + (((unsigned)sig[1]) << 8)
                      + (sig[0]));

#endif /* ?LARGE_FILE_SUPPORT */
}