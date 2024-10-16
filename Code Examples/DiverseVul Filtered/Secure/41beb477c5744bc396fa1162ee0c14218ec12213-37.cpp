ulg makelong(sig)
    ZCONST uch *sig;
{
    /*
     * Convert intel style 'long' variable to non-Intel non-16-bit
     * host format.  This routine also takes care of byte-ordering.
     */
    return (((ulg)sig[3]) << 24)
         + (((ulg)sig[2]) << 16)
         + (ulg)((((unsigned)sig[1]) << 8)
               + ((unsigned)sig[0]));
}