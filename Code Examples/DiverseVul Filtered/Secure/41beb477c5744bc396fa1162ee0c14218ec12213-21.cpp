int flush(__G__ rawbuf, size, unshrink)
    __GDEF
    uch *rawbuf;
    ulg size;
    int unshrink;
#if (defined(USE_DEFLATE64) && defined(__16BIT__))
{
    int ret;

    /* On 16-bit systems (MSDOS, OS/2 1.x), the standard C library functions
     * cannot handle writes of 64k blocks at once.  For these systems, the
     * blocks to flush are split into pieces of 32k or less.
     */
    while (size > 0x8000L) {
        ret = partflush(__G__ rawbuf, 0x8000L, unshrink);
        if (ret != PK_OK)
            return ret;
        size -= 0x8000L;
        rawbuf += (extent)0x8000;
    }
    return partflush(__G__ rawbuf, size, unshrink);
} /* end function flush() */