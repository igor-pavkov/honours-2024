int CLASS phase_one_correct()
{
  unsigned entries, tag, data, save, col, row, type;
  int len, i, j, k, cip, val[4], dev[4], sum, max;
  int head[9], diff, mindiff = INT_MAX, off_412 = 0;
  /* static */ const signed char dir[12][2] = {{-1, -1}, {-1, 1}, {1, -1},  {1, 1},  {-2, 0}, {0, -2},
                                               {0, 2},   {2, 0},  {-2, -2}, {-2, 2}, {2, -2}, {2, 2}};
  float poly[8], num, cfrac, frac, mult[2], *yval[2] = {NULL, NULL};
  ushort *xval[2];
  int qmult_applied = 0, qlin_applied = 0;

#ifdef LIBRAW_LIBRARY_BUILD
  if (!meta_length)
#else
  if (half_size || !meta_length)
#endif
    return 0;
#ifdef DCRAW_VERBOSE
  if (verbose)
    fprintf(stderr, _("Phase One correction...\n"));
#endif
  fseek(ifp, meta_offset, SEEK_SET);
  order = get2();
  fseek(ifp, 6, SEEK_CUR);
  fseek(ifp, meta_offset + get4(), SEEK_SET);
  entries = get4();
  get4();

#ifdef LIBRAW_LIBRARY_BUILD
  try
  {
#endif
    while (entries--)
    {
#ifdef LIBRAW_LIBRARY_BUILD
      checkCancel();
#endif
      tag = get4();
      len = get4();
      data = get4();
      save = ftell(ifp);
      fseek(ifp, meta_offset + data, SEEK_SET);
      if (tag == 0x419)
      { /* Polynomial curve */
        for (get4(), i = 0; i < 8; i++)
          poly[i] = getreal(11);
        poly[3] += (ph1.tag_210 - poly[7]) * poly[6] + 1;
        for (i = 0; i < 0x10000; i++)
        {
          num = (poly[5] * i + poly[3]) * i + poly[1];
          curve[i] = LIM(num, 0, 65535);
        }
        goto apply; /* apply to right half */
      }
      else if (tag == 0x41a)
      { /* Polynomial curve */
        for (i = 0; i < 4; i++)
          poly[i] = getreal(11);
        for (i = 0; i < 0x10000; i++)
        {
          for (num = 0, j = 4; j--;)
            num = num * i + poly[j];
          curve[i] = LIM(num + i, 0, 65535);
        }
      apply: /* apply to whole image */
        for (row = 0; row < raw_height; row++)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          checkCancel();
#endif
          for (col = (tag & 1) * ph1.split_col; col < raw_width; col++)
            RAW(row, col) = curve[RAW(row, col)];
        }
      }
      else if (tag == 0x400)
      { /* Sensor defects */
        while ((len -= 8) >= 0)
        {
          col = get2();
          row = get2();
          type = get2();
          get2();
          if (col >= raw_width)
            continue;
          if (type == 131 || type == 137) /* Bad column */
            for (row = 0; row < raw_height; row++)
              if (FC(row - top_margin, col - left_margin) == 1)
              {
                for (sum = i = 0; i < 4; i++)
                  sum += val[i] = raw(row + dir[i][0], col + dir[i][1]);
                for (max = i = 0; i < 4; i++)
                {
                  dev[i] = abs((val[i] << 2) - sum);
                  if (dev[max] < dev[i])
                    max = i;
                }
                RAW(row, col) = (sum - val[max]) / 3.0 + 0.5;
              }
              else
              {
                for (sum = 0, i = 8; i < 12; i++)
                  sum += raw(row + dir[i][0], col + dir[i][1]);
                RAW(row, col) = 0.5 + sum * 0.0732233 + (raw(row, col - 2) + raw(row, col + 2)) * 0.3535534;
              }
          else if (type == 129)
          { /* Bad pixel */
            if (row >= raw_height)
              continue;
            j = (FC(row - top_margin, col - left_margin) != 1) * 4;
            for (sum = 0, i = j; i < j + 8; i++)
              sum += raw(row + dir[i][0], col + dir[i][1]);
            RAW(row, col) = (sum + 4) >> 3;
          }
        }
      }
      else if (tag == 0x401)
      { /* All-color flat fields */
        phase_one_flat_field(1, 2);
      }
      else if (tag == 0x416 || tag == 0x410)
      {
        phase_one_flat_field(0, 2);
      }
      else if (tag == 0x40b)
      { /* Red+blue flat field */
        phase_one_flat_field(0, 4);
      }
      else if (tag == 0x412)
      {
        fseek(ifp, 36, SEEK_CUR);
        diff = abs(get2() - ph1.tag_21a);
        if (mindiff > diff)
        {
          mindiff = diff;
          off_412 = ftell(ifp) - 38;
        }
      }
      else if (tag == 0x41f && !qlin_applied)
      { /* Quadrant linearization */
        ushort lc[2][2][16], ref[16];
        int qr, qc;
        for (qr = 0; qr < 2; qr++)
          for (qc = 0; qc < 2; qc++)
            for (i = 0; i < 16; i++)
              lc[qr][qc][i] = get4();
        for (i = 0; i < 16; i++)
        {
          int v = 0;
          for (qr = 0; qr < 2; qr++)
            for (qc = 0; qc < 2; qc++)
              v += lc[qr][qc][i];
          ref[i] = (v + 2) >> 2;
        }
        for (qr = 0; qr < 2; qr++)
        {
          for (qc = 0; qc < 2; qc++)
          {
            int cx[19], cf[19];
            for (i = 0; i < 16; i++)
            {
              cx[1 + i] = lc[qr][qc][i];
              cf[1 + i] = ref[i];
            }
            cx[0] = cf[0] = 0;
            cx[17] = cf[17] = ((unsigned int)ref[15] * 65535) / lc[qr][qc][15];
            cf[18] = cx[18] = 65535;
            cubic_spline(cx, cf, 19);

            for (row = (qr ? ph1.split_row : 0); row < (qr ? raw_height : ph1.split_row); row++)
            {
#ifdef LIBRAW_LIBRARY_BUILD
              checkCancel();
#endif
              for (col = (qc ? ph1.split_col : 0); col < (qc ? raw_width : ph1.split_col); col++)
                RAW(row, col) = curve[RAW(row, col)];
            }
          }
        }
        qlin_applied = 1;
      }
      else if (tag == 0x41e && !qmult_applied)
      { /* Quadrant multipliers */
        float qmult[2][2] = {{1, 1}, {1, 1}};
        get4();
        get4();
        get4();
        get4();
        qmult[0][0] = 1.0 + getreal(11);
        get4();
        get4();
        get4();
        get4();
        get4();
        qmult[0][1] = 1.0 + getreal(11);
        get4();
        get4();
        get4();
        qmult[1][0] = 1.0 + getreal(11);
        get4();
        get4();
        get4();
        qmult[1][1] = 1.0 + getreal(11);
        for (row = 0; row < raw_height; row++)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          checkCancel();
#endif
          for (col = 0; col < raw_width; col++)
          {
            i = qmult[row >= ph1.split_row][col >= ph1.split_col] * RAW(row, col);
            RAW(row, col) = LIM(i, 0, 65535);
          }
        }
        qmult_applied = 1;
      }
      else if (tag == 0x431 && !qmult_applied)
      { /* Quadrant combined */
        ushort lc[2][2][7], ref[7];
        int qr, qc;
        for (i = 0; i < 7; i++)
          ref[i] = get4();
        for (qr = 0; qr < 2; qr++)
          for (qc = 0; qc < 2; qc++)
            for (i = 0; i < 7; i++)
              lc[qr][qc][i] = get4();
        for (qr = 0; qr < 2; qr++)
        {
          for (qc = 0; qc < 2; qc++)
          {
            int cx[9], cf[9];
            for (i = 0; i < 7; i++)
            {
              cx[1 + i] = ref[i];
              cf[1 + i] = ((unsigned)ref[i] * lc[qr][qc][i]) / 10000;
            }
            cx[0] = cf[0] = 0;
            cx[8] = cf[8] = 65535;
            cubic_spline(cx, cf, 9);
            for (row = (qr ? ph1.split_row : 0); row < (qr ? raw_height : ph1.split_row); row++)
            {
#ifdef LIBRAW_LIBRARY_BUILD
              checkCancel();
#endif
              for (col = (qc ? ph1.split_col : 0); col < (qc ? raw_width : ph1.split_col); col++)
                RAW(row, col) = curve[RAW(row, col)];
            }
          }
        }
        qmult_applied = 1;
        qlin_applied = 1;
      }
      fseek(ifp, save, SEEK_SET);
    }
    if (off_412)
    {
      fseek(ifp, off_412, SEEK_SET);
      for (i = 0; i < 9; i++)
        head[i] = get4() & 0x7fff;
      yval[0] = (float *)calloc(head[1] * head[3] + head[2] * head[4], 6);
      merror(yval[0], "phase_one_correct()");
      yval[1] = (float *)(yval[0] + head[1] * head[3]);
      xval[0] = (ushort *)(yval[1] + head[2] * head[4]);
      xval[1] = (ushort *)(xval[0] + head[1] * head[3]);
      get2();
      for (i = 0; i < 2; i++)
        for (j = 0; j < head[i + 1] * head[i + 3]; j++)
          yval[i][j] = getreal(11);
      for (i = 0; i < 2; i++)
        for (j = 0; j < head[i + 1] * head[i + 3]; j++)
          xval[i][j] = get2();
      for (row = 0; row < raw_height; row++)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        checkCancel();
#endif
        for (col = 0; col < raw_width; col++)
        {
          cfrac = (float)col * head[3] / raw_width;
          cfrac -= cip = cfrac;
          num = RAW(row, col) * 0.5;
          for (i = cip; i < cip + 2; i++)
          {
            for (k = j = 0; j < head[1]; j++)
              if (num < xval[0][k = head[1] * i + j])
                break;
            frac = (j == 0 || j == head[1]) ? 0 : (xval[0][k] - num) / (xval[0][k] - xval[0][k - 1]);
            mult[i - cip] = yval[0][k - 1] * frac + yval[0][k] * (1 - frac);
          }
          i = ((mult[0] * (1 - cfrac) + mult[1] * cfrac) * row + num) * 2;
          RAW(row, col) = LIM(i, 0, 65535);
        }
      }
      free(yval[0]);
    }
#ifdef LIBRAW_LIBRARY_BUILD
  }
  catch (...)
  {
    if (yval[0])
      free(yval[0]);
    return LIBRAW_CANCELLED_BY_CALLBACK;
  }
#endif
  return 0;
}