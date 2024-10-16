int check_for_newer(__G__ filename)  /* return 1 if existing file is newer */
    __GDEF                           /*  or equal; 0 if older; -1 if doesn't */
    char *filename;                  /*  exist yet */
{
    time_t existing, archive;
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
#endif
#ifdef AOS_VS
    long    dyy, dmm, ddd, dhh, dmin, dss;


    dyy = (lrec.last_mod_dos_datetime >> 25) + 1980;
    dmm = (lrec.last_mod_dos_datetime >> 21) & 0x0f;
    ddd = (lrec.last_mod_dos_datetime >> 16) & 0x1f;
    dhh = (lrec.last_mod_dos_datetime >> 11) & 0x1f;
    dmin = (lrec.last_mod_dos_datetime >> 5) & 0x3f;
    dss = (lrec.last_mod_dos_datetime & 0x1f) * 2;

    /* under AOS/VS, file times can only be set at creation time,
     * with the info in a special DG format.  Make sure we can create
     * it here - we delete it later & re-create it, whether or not
     * it exists now.
     */
    if (!zvs_create(filename, (((ulg)dgdate(dmm, ddd, dyy)) << 16) |
        (dhh*1800L + dmin*30L + dss/2L), -1L, -1L, (char *) -1, -1, -1, -1))
        return DOES_NOT_EXIST;
#endif /* AOS_VS */

    Trace((stderr, "check_for_newer:  doing stat(%s)\n", FnFilter1(filename)));
    if (SSTAT(filename, &G.statbuf)) {
        Trace((stderr,
          "check_for_newer:  stat(%s) returns %d:  file does not exist\n",
          FnFilter1(filename), SSTAT(filename, &G.statbuf)));
#ifdef SYMLINKS
        Trace((stderr, "check_for_newer:  doing lstat(%s)\n",
          FnFilter1(filename)));
        /* GRR OPTION:  could instead do this test ONLY if G.symlnk is true */
        if (lstat(filename, &G.statbuf) == 0) {
            Trace((stderr,
              "check_for_newer:  lstat(%s) returns 0:  symlink does exist\n",
              FnFilter1(filename)));
            if (QCOND2 && !IS_OVERWRT_ALL)
                Info(slide, 0, ((char *)slide, LoadFarString(FileIsSymLink),
                  FnFilter1(filename), " with no real file"));
            return EXISTS_AND_OLDER;   /* symlink dates are meaningless */
        }
#endif /* SYMLINKS */
        return DOES_NOT_EXIST;
    }
    Trace((stderr, "check_for_newer:  stat(%s) returns 0:  file exists\n",
      FnFilter1(filename)));

#ifdef SYMLINKS
    /* GRR OPTION:  could instead do this test ONLY if G.symlnk is true */
    if (lstat(filename, &G.statbuf) == 0 && S_ISLNK(G.statbuf.st_mode)) {
        Trace((stderr, "check_for_newer:  %s is a symbolic link\n",
          FnFilter1(filename)));
        if (QCOND2 && !IS_OVERWRT_ALL)
            Info(slide, 0, ((char *)slide, LoadFarString(FileIsSymLink),
              FnFilter1(filename), ""));
        return EXISTS_AND_OLDER;   /* symlink dates are meaningless */
    }
#endif /* SYMLINKS */

    NATIVE_TO_TIMET(G.statbuf.st_mtime)   /* NOP unless MSC 7.0 or Macintosh */

#ifdef USE_EF_UT_TIME
    /* The `Unix extra field mtime' should be used for comparison with the
     * time stamp of the existing file >>>ONLY<<< when the EF info is also
     * used to set the modification time of the extracted file.
     */
    if (G.extra_field &&
#ifdef IZ_CHECK_TZ
        G.tz_is_valid &&
#endif
        (ef_scan_for_izux(G.extra_field, G.lrec.extra_field_length, 0,
                          G.lrec.last_mod_dos_datetime, &z_utime, NULL)
         & EB_UT_FL_MTIME))
    {
        TTrace((stderr, "check_for_newer:  using Unix extra field mtime\n"));
        existing = G.statbuf.st_mtime;
        archive  = z_utime.mtime;
    } else {
        /* round up existing filetime to nearest 2 seconds for comparison,
         * but saturate in case of arithmetic overflow
         */
        existing = ((G.statbuf.st_mtime & 1) &&
                    (G.statbuf.st_mtime + 1 > G.statbuf.st_mtime)) ?
                   G.statbuf.st_mtime + 1 : G.statbuf.st_mtime;
        archive  = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
    }
#else /* !USE_EF_UT_TIME */
    /* round up existing filetime to nearest 2 seconds for comparison,
     * but saturate in case of arithmetic overflow
     */
    existing = ((G.statbuf.st_mtime & 1) &&
                (G.statbuf.st_mtime + 1 > G.statbuf.st_mtime)) ?
               G.statbuf.st_mtime + 1 : G.statbuf.st_mtime;
    archive  = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
#endif /* ?USE_EF_UT_TIME */

    TTrace((stderr, "check_for_newer:  existing %lu, archive %lu, e-a %ld\n",
      (ulg)existing, (ulg)archive, (long)(existing-archive)));

    return (existing >= archive);

} /* end function check_for_newer() */