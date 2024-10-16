void CLASS lossless_dng_load_raw()
{
  unsigned save, trow = 0, tcol = 0, jwide, jrow, jcol, row, col, i, j;
  struct jhead jh;
  ushort *rp;

  while (trow < raw_height)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    save = ftell(ifp);
    if (tile_length < INT_MAX)
      fseek(ifp, get4(), SEEK_SET);
    if (!ljpeg_start(&jh, 0))
      break;
    jwide = jh.wide;
    if (filters)
      jwide *= jh.clrs;
    jwide /= MIN(is_raw, tiff_samples);
#ifdef LIBRAW_LIBRARY_BUILD
    try
    {
#endif
      switch (jh.algo)
      {
      case 0xc1:
        jh.vpred[0] = 16384;
        getbits(-1);
        for (jrow = 0; jrow + 7 < jh.high; jrow += 8)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          checkCancel();
#endif
          for (jcol = 0; jcol + 7 < jh.wide; jcol += 8)
          {
            ljpeg_idct(&jh);
            rp = jh.idct;
            row = trow + jcol / tile_width + jrow * 2;
            col = tcol + jcol % tile_width;
            for (i = 0; i < 16; i += 2)
              for (j = 0; j < 8; j++)
                adobe_copy_pixel(row + i, col + j, &rp);
          }
        }
        break;
      case 0xc3:
        for (row = col = jrow = 0; jrow < jh.high; jrow++)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          checkCancel();
#endif
          rp = ljpeg_row(jrow, &jh);
	  if(tiff_samples == 1 && jh.clrs > 1 && jh.clrs*jwide == raw_width)
          for (jcol = 0; jcol < jwide*jh.clrs; jcol++)
          {
            adobe_copy_pixel(trow + row, tcol + col, &rp);
            if (++col >= tile_width || col >= raw_width)
              row += 1 + (col = 0);
	  }
	  else
          for (jcol = 0; jcol < jwide; jcol++)
          {
            adobe_copy_pixel(trow + row, tcol + col, &rp);
            if (++col >= tile_width || col >= raw_width)
              row += 1 + (col = 0);
          }
        }
      }
#ifdef LIBRAW_LIBRARY_BUILD
    }
    catch (...)
    {
      ljpeg_end(&jh);
      throw;
    }
#endif
    fseek(ifp, save + 4, SEEK_SET);
    if ((tcol += tile_width) >= raw_width)
      trow += tile_length + (tcol = 0);
    ljpeg_end(&jh);
  }
}