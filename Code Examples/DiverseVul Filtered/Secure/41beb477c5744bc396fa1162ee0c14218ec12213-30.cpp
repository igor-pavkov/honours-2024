static int disk_error(__G)
    __GDEF
{
    /* OK to use slide[] here because this file is finished regardless */
    Info(slide, 0x4a1, ((char *)slide, LoadFarString(DiskFullQuery),
      FnFilter1(G.filename)));

#ifndef WINDLL
    fgets(G.answerbuf, sizeof(G.answerbuf), stdin);
    if (*G.answerbuf == 'y')   /* stop writing to this file */
        G.disk_full = 1;       /*  (outfile bad?), but new OK */
    else
#endif
        G.disk_full = 2;       /* no:  exit program */

    return PK_DISK;

} /* end function disk_error() */