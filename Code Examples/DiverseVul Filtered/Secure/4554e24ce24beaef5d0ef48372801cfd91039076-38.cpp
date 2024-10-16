void CLASS parse_kodak_ifd(int base)
{
  unsigned entries, tag, type, len, save;
  int j, c, wbi = -2, romm_camTemp[9], romm_camScale[3];
  float mul[3] = {1, 1, 1}, num;
  static const int wbtag[] = {64037, 64040, 64039, 64041, -1, -1, 64042};
  //  int a_blck = 0;

  entries = get2();
  if (entries > 1024)
    return;
  INT64 fsize = ifp->size();
  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    INT64 savepos = ftell(ifp);
    if (len > 8 && len + savepos > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    if (callbacks.exif_cb)
    {
      callbacks.exif_cb(callbacks.exifparser_data, tag | 0x20000, type, len, order, ifp);
      fseek(ifp, savepos, SEEK_SET);
    }
    if (tag == 1003)
      imgdata.sizes.raw_crop.cleft = get2();
    if (tag == 1004)
      imgdata.sizes.raw_crop.ctop = get2();
    if (tag == 1005)
      imgdata.sizes.raw_crop.cwidth = get2();
    if (tag == 1006)
      imgdata.sizes.raw_crop.cheight = get2();
    if (tag == 1007)
      imgdata.makernotes.kodak.BlackLevelTop = get2();
    if (tag == 1008)
      imgdata.makernotes.kodak.BlackLevelBottom = get2();
    if (tag == 1011)
      imgdata.other.FlashEC = getreal(type);
    if (tag == 1020)
      wbi = getint(type);
    if (tag == 1021 && len == 72)
    { /* WB set in software */
      fseek(ifp, 40, SEEK_CUR);
      FORC3 cam_mul[c] = 2048.0 / fMAX(1.0f, get2());
      wbi = -2;
    }

    if ((tag == 1030) && (len == 1))
      imgdata.other.CameraTemperature = getreal(type);
    if ((tag == 1043) && (len == 1))
      imgdata.other.SensorTemperature = getreal(type);

    if ((tag == 0x03ef) && (!strcmp(model, "EOS D2000C")))
      black = get2();
    if ((tag == 0x03f0) && (!strcmp(model, "EOS D2000C")))
    {
      if (black) // already set by tag 0x03ef
        black = (black + get2()) / 2;
      else
        black = get2();
    }
    INT64 _pos2 = ftell(ifp);
    if (tag == 0x0848)
      Kodak_WB_0x08tags(LIBRAW_WBI_Daylight, type);
    if (tag == 0x0849)
      Kodak_WB_0x08tags(LIBRAW_WBI_Tungsten, type);
    if (tag == 0x084a)
      Kodak_WB_0x08tags(LIBRAW_WBI_Fluorescent, type);
    if (tag == 0x084b)
      Kodak_WB_0x08tags(LIBRAW_WBI_Flash, type);
    if (tag == 0x084c)
      Kodak_WB_0x08tags(LIBRAW_WBI_Custom, type);
    if (tag == 0x084d)
      Kodak_WB_0x08tags(LIBRAW_WBI_Auto, type);
    if (tag == 0x0e93)
      imgdata.color.linear_max[0] = imgdata.color.linear_max[1] = imgdata.color.linear_max[2] =
          imgdata.color.linear_max[3] = get2();
    if (tag == 0x09ce)
      stmread(imgdata.shootinginfo.InternalBodySerial, len, ifp);
    if (tag == 0xfa00)
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
    if (tag == 0xfa27)
    {
      FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c] = get4();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][3] = imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][1];
    }
    if (tag == 0xfa28)
    {
      FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Fluorescent][c] = get4();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Fluorescent][3] = imgdata.color.WB_Coeffs[LIBRAW_WBI_Fluorescent][1];
    }
    if (tag == 0xfa29)
    {
      FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Daylight][c] = get4();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Daylight][3] = imgdata.color.WB_Coeffs[LIBRAW_WBI_Daylight][1];
    }
    if (tag == 0xfa2a)
    {
      FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][c] = get4();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][3] = imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][1];
    }

    fseek(ifp, _pos2, SEEK_SET);

    if (((tag == 0x07e4) || (tag == 0xfb01)) && (len == 9))
    {
      short validM = 0;
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camDaylight)[j] = getreal(type);
        }
        validM = 1;
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camDaylight)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
          validM = 1;
        }
      }
      if (validM)
      {
        romm_coeff(imgdata.makernotes.kodak.romm_camDaylight);
      }
    }
    if (((tag == 0x07e5) || (tag == 0xfb02)) && (len == 9))
    {
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camTungsten)[j] = getreal(type);
        }
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camTungsten)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
        }
      }
    }
    if (((tag == 0x07e6) || (tag == 0xfb03)) && (len == 9))
    {
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camFluorescent)[j] = getreal(type);
        }
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camFluorescent)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
        }
      }
    }
    if (((tag == 0x07e7) || (tag == 0xfb04)) && (len == 9))
    {
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camFlash)[j] = getreal(type);
        }
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camFlash)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
        }
      }
    }
    if (((tag == 0x07e8) || (tag == 0xfb05)) && (len == 9))
    {
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camCustom)[j] = getreal(type);
        }
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camCustom)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
        }
      }
    }
    if (((tag == 0x07e9) || (tag == 0xfb06)) && (len == 9))
    {
      if (type == 10)
      {
        for (j = 0; j < 9; j++)
        {
          ((float *)imgdata.makernotes.kodak.romm_camAuto)[j] = getreal(type);
        }
      }
      else if (type == 9)
      {
        FORC3
        {
          romm_camScale[c] = 0;
          for (j = 0; j < 3; j++)
          {
            romm_camTemp[c * 3 + j] = get4();
            romm_camScale[c] += romm_camTemp[c * 3 + j];
          }
        }
        if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) && (romm_camScale[2] > 0x1fff))
        {
          FORC3 for (j = 0; j < 3; j++)
          {
            ((float *)imgdata.makernotes.kodak.romm_camAuto)[c * 3 + j] =
                ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
          }
        }
      }
    }

    if (tag == 2120 + wbi || (wbi < 0 && tag == 2125)) /* use Auto WB if illuminant index is not set */
    {
      FORC3 mul[c] = (num = getreal(type)) == 0 ? 1 : num;
      FORC3 cam_mul[c] = mul[1] / mul[c]; /* normalise against green */
    }
    if (tag == 2317)
      linear_table(len);
    if (tag == 0x903)
      iso_speed = getreal(type);
    // if (tag == 6020) iso_speed = getint(type);
    if (tag == 64013)
      wbi = fgetc(ifp);
    if ((unsigned)wbi < 7 && tag == wbtag[wbi])
      FORC3 cam_mul[c] = get4();
    if (tag == 0xfa13)
      width = getint(type);
    if (tag == 0xfa14)
      height = (getint(type) + 1) & -2;

    /*
        height = getint(type);

        if (tag == 0xfa16)
          raw_width = get2();
        if (tag == 0xfa17)
          raw_height = get2();
    */
    if (tag == 0xfa18)
    {
      imgdata.makernotes.kodak.offset_left = getint(8);
      if (type != 8)
        imgdata.makernotes.kodak.offset_left += 1;
    }
    if (tag == 0xfa19)
    {
      imgdata.makernotes.kodak.offset_top = getint(8);
      if (type != 8)
        imgdata.makernotes.kodak.offset_top += 1;
    }

    if (tag == 0xfa31)
      imgdata.sizes.raw_crop.cwidth = get2();
    if (tag == 0xfa32)
      imgdata.sizes.raw_crop.cheight = get2();
    if (tag == 0xfa3e)
      imgdata.sizes.raw_crop.cleft = get2();
    if (tag == 0xfa3f)
      imgdata.sizes.raw_crop.ctop = get2();

    fseek(ifp, save, SEEK_SET);
  }
}