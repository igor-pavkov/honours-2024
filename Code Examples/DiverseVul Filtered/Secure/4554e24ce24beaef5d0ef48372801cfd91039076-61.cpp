void CLASS canon_sraw_load_raw()
{
  struct jhead jh;
  short *rp = 0, (*ip)[4];
  int jwide, slice, scol, ecol, row, col, jrow = 0, jcol = 0, pix[3], c;
  int v[3] = {0, 0, 0}, ver, hue;
#ifdef LIBRAW_LIBRARY_BUILD
  int saved_w = width, saved_h = height;
#endif
  char *cp;

  if (!ljpeg_start(&jh, 0) || jh.clrs < 4)
    return;
  jwide = (jh.wide >>= 1) * jh.clrs;

#ifdef LIBRAW_LIBRARY_BUILD
  if (load_flags & 256)
  {
    width = raw_width;
    height = raw_height;
  }

  try
  {
#endif
    for (ecol = slice = 0; slice <= cr2_slice[0]; slice++)
    {
      scol = ecol;
      ecol += cr2_slice[1] * 2 / jh.clrs;
      if (!cr2_slice[0] || ecol > raw_width - 1)
        ecol = raw_width & -2;
      for (row = 0; row < height; row += (jh.clrs >> 1) - 1)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        checkCancel();
#endif
        ip = (short(*)[4])image + row * width;
        for (col = scol; col < ecol; col += 2, jcol += jh.clrs)
        {
          if ((jcol %= jwide) == 0)
            rp = (short *)ljpeg_row(jrow++, &jh);
          if (col >= width)
            continue;
#ifdef LIBRAW_LIBRARY_BUILD
          if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SRAW_NO_INTERPOLATE)
          {
            FORC(jh.clrs - 2)
            {
              ip[col + (c >> 1) * width + (c & 1)][0] = rp[jcol + c];
              ip[col + (c >> 1) * width + (c & 1)][1] = ip[col + (c >> 1) * width + (c & 1)][2] = 8192;
            }
            ip[col][1] = rp[jcol + jh.clrs - 2] - 8192;
            ip[col][2] = rp[jcol + jh.clrs - 1] - 8192;
          }
          else if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SRAW_NO_RGB)
          {
            FORC(jh.clrs - 2)
            ip[col + (c >> 1) * width + (c & 1)][0] = rp[jcol + c];
            ip[col][1] = rp[jcol + jh.clrs - 2] - 8192;
            ip[col][2] = rp[jcol + jh.clrs - 1] - 8192;
          }
          else
#endif
          {
            FORC(jh.clrs - 2)
            ip[col + (c >> 1) * width + (c & 1)][0] = rp[jcol + c];
            ip[col][1] = rp[jcol + jh.clrs - 2] - 16384;
            ip[col][2] = rp[jcol + jh.clrs - 1] - 16384;
          }
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

#ifdef LIBRAW_LIBRARY_BUILD
  if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SRAW_NO_INTERPOLATE)
  {
    ljpeg_end(&jh);
    maximum = 0x3fff;
    height = saved_h;
    width = saved_w;
    return;
  }
#endif

#ifdef LIBRAW_LIBRARY_BUILD
  try
  {
#endif
    for (cp = model2; *cp && !isdigit(*cp); cp++)
      ;
    sscanf(cp, "%d.%d.%d", v, v + 1, v + 2);
    ver = (v[0] * 1000 + v[1]) * 1000 + v[2];
    hue = (jh.sraw + 1) << 2;
    if (unique_id >= 0x80000281 || (unique_id == 0x80000218 && ver > 1000006))
      hue = jh.sraw << 1;
    ip = (short(*)[4])image;
    rp = ip[0];
    for (row = 0; row < height; row++, ip += width)
    {
#ifdef LIBRAW_LIBRARY_BUILD
      checkCancel();
#endif
      if (row & (jh.sraw >> 1))
      {
        for (col = 0; col < width; col += 2)
          for (c = 1; c < 3; c++)
            if (row == height - 1)
            {
              ip[col][c] = ip[col - width][c];
            }
            else
            {
              ip[col][c] = (ip[col - width][c] + ip[col + width][c] + 1) >> 1;
            }
      }
      for (col = 1; col < width; col += 2)
        for (c = 1; c < 3; c++)
          if (col == width - 1)
            ip[col][c] = ip[col - 1][c];
          else
            ip[col][c] = (ip[col - 1][c] + ip[col + 1][c] + 1) >> 1;
    }
#ifdef LIBRAW_LIBRARY_BUILD
    if (!(imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SRAW_NO_RGB))
#endif
      for (; rp < ip[0]; rp += 4)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        checkCancel();
#endif
        if (unique_id == 0x80000218 || unique_id == 0x80000250 || unique_id == 0x80000261 || unique_id == 0x80000281 ||
            unique_id == 0x80000287)
        {
          rp[1] = (rp[1] << 2) + hue;
          rp[2] = (rp[2] << 2) + hue;
          pix[0] = rp[0] + ((50 * rp[1] + 22929 * rp[2]) >> 14);
          pix[1] = rp[0] + ((-5640 * rp[1] - 11751 * rp[2]) >> 14);
          pix[2] = rp[0] + ((29040 * rp[1] - 101 * rp[2]) >> 14);
        }
        else
        {
          if (unique_id < 0x80000218)
            rp[0] -= 512;
          pix[0] = rp[0] + rp[2];
          pix[2] = rp[0] + rp[1];
          pix[1] = rp[0] + ((-778 * rp[1] - (rp[2] << 11)) >> 12);
        }
        FORC3 rp[c] = CLIP15(pix[c] * sraw_mul[c] >> 10);
      }
#ifdef LIBRAW_LIBRARY_BUILD
  }
  catch (...)
  {
    ljpeg_end(&jh);
    throw;
  }
  height = saved_h;
  width = saved_w;
#endif
  ljpeg_end(&jh);
  maximum = 0x3fff;
}