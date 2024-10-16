void CLASS PentaxLensInfo(unsigned id, unsigned len) // tag 0x0207
{
  ushort iLensData = 0;
  uchar *table_buf;
  table_buf = (uchar *)malloc(MAX(len, 128));
  fread(table_buf, len, 1, ifp);
  if ((id < 0x12b9c) || (((id == 0x12b9c) ||  // K100D
                          (id == 0x12b9d) ||  // K110D
                          (id == 0x12ba2)) && // K100D Super
                         ((!table_buf[20] || (table_buf[20] == 0xff)))))
  {
    iLensData = 3;
    if (imgdata.lens.makernotes.LensID == -1)
      imgdata.lens.makernotes.LensID = (((unsigned)table_buf[0]) << 8) + table_buf[1];
  }
  else
    switch (len)
    {
    case 90: // LensInfo3
      iLensData = 13;
      if (imgdata.lens.makernotes.LensID == -1)
        imgdata.lens.makernotes.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) << 8) + table_buf[4];
      break;
    case 91: // LensInfo4
      iLensData = 12;
      if (imgdata.lens.makernotes.LensID == -1)
        imgdata.lens.makernotes.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) << 8) + table_buf[4];
      break;
    case 80: // LensInfo5
    case 128:
      iLensData = 15;
      if (imgdata.lens.makernotes.LensID == -1)
        imgdata.lens.makernotes.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[4]) << 8) + table_buf[5];
      break;
    default:
      if (id >= 0x12b9c) // LensInfo2
      {
        iLensData = 4;
        if (imgdata.lens.makernotes.LensID == -1)
          imgdata.lens.makernotes.LensID = ((unsigned)((table_buf[0] & 0x0f) + table_buf[2]) << 8) + table_buf[3];
      }
    }
  if (iLensData)
  {
    if (table_buf[iLensData + 9] && (fabs(imgdata.lens.makernotes.CurFocal) < 0.1f))
      imgdata.lens.makernotes.CurFocal =
          10 * (table_buf[iLensData + 9] >> 2) * libraw_powf64l(4, (table_buf[iLensData + 9] & 0x03) - 2);
    if (table_buf[iLensData + 10] & 0xf0)
      imgdata.lens.makernotes.MaxAp4CurFocal =
          libraw_powf64l(2.0f, (float)((table_buf[iLensData + 10] & 0xf0) >> 4) / 4.0f);
    if (table_buf[iLensData + 10] & 0x0f)
      imgdata.lens.makernotes.MinAp4CurFocal =
          libraw_powf64l(2.0f, (float)((table_buf[iLensData + 10] & 0x0f) + 10) / 4.0f);

    if (iLensData != 12)
    {
      switch (table_buf[iLensData] & 0x06)
      {
      case 0:
        imgdata.lens.makernotes.MinAp4MinFocal = 22.0f;
        break;
      case 2:
        imgdata.lens.makernotes.MinAp4MinFocal = 32.0f;
        break;
      case 4:
        imgdata.lens.makernotes.MinAp4MinFocal = 45.0f;
        break;
      case 6:
        imgdata.lens.makernotes.MinAp4MinFocal = 16.0f;
        break;
      }
      if (table_buf[iLensData] & 0x70)
        imgdata.lens.makernotes.LensFStops = ((float)(((table_buf[iLensData] & 0x70) >> 4) ^ 0x07)) / 2.0f + 5.0f;

      imgdata.lens.makernotes.MinFocusDistance = (float)(table_buf[iLensData + 3] & 0xf8);
      imgdata.lens.makernotes.FocusRangeIndex = (float)(table_buf[iLensData + 3] & 0x07);

      if ((table_buf[iLensData + 14] > 1) && (fabs(imgdata.lens.makernotes.MaxAp4CurFocal) < 0.7f))
        imgdata.lens.makernotes.MaxAp4CurFocal =
            libraw_powf64l(2.0f, (float)((table_buf[iLensData + 14] & 0x7f) - 1) / 32.0f);
    }
    else if ((id != 0x12e76) && // K-5
             (table_buf[iLensData + 15] > 1) && (fabs(imgdata.lens.makernotes.MaxAp4CurFocal) < 0.7f))
    {
      imgdata.lens.makernotes.MaxAp4CurFocal =
          libraw_powf64l(2.0f, (float)((table_buf[iLensData + 15] & 0x7f) - 1) / 32.0f);
    }
  }
  free(table_buf);
  return;
}