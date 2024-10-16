void CLASS process_Sony_0x9050(uchar *buf, ushort len, unsigned id)
{
  ushort lid;
  uchar s[4];
  int c;

  if ((imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_Sony_E) &&
      (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens))
  {
    if (len < 2)
      return;
    if (buf[0])
      imgdata.lens.makernotes.MaxAp4CurFocal =
          my_roundf(libraw_powf64l(2.0f, ((float)SonySubstitution[buf[0]] / 8.0 - 1.06f) / 2.0f) * 10.0f) / 10.0f;

    if (buf[1])
      imgdata.lens.makernotes.MinAp4CurFocal =
          my_roundf(libraw_powf64l(2.0f, ((float)SonySubstitution[buf[1]] / 8.0 - 1.06f) / 2.0f) * 10.0f) / 10.0f;
  }

  if (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens)
  {
    if (len <= 0x106)
      return;
    if (buf[0x3d] | buf[0x3c])
    {
      lid = SonySubstitution[buf[0x3d]] << 8 | SonySubstitution[buf[0x3c]];
      imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, ((float)lid / 256.0f - 16.0f) / 2.0f);
    }
    if (buf[0x105] && (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Canon_EF) &&
        (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Sigma_X3F))
      imgdata.lens.makernotes.LensMount = SonySubstitution[buf[0x105]];
    if (buf[0x106])
      imgdata.lens.makernotes.LensFormat = SonySubstitution[buf[0x106]];
  }

  if (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E)
  {
    if (len <= 0x108)
      return;
    parseSonyLensType2(SonySubstitution[buf[0x0108]], // LensType2 - Sony lens ids
                       SonySubstitution[buf[0x0107]]);
  }

  if (len <= 0x10a)
    return;
  if ((imgdata.lens.makernotes.LensID == -1) && (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Minolta_A) &&
      (buf[0x010a] | buf[0x0109]))
  {
    imgdata.lens.makernotes.LensID = // LensType - Minolta/Sony lens ids
        SonySubstitution[buf[0x010a]] << 8 | SonySubstitution[buf[0x0109]];

    if ((imgdata.lens.makernotes.LensID > 0x4900) && (imgdata.lens.makernotes.LensID <= 0x5900))
    {
      imgdata.lens.makernotes.AdapterID = 0x4900;
      imgdata.lens.makernotes.LensID -= imgdata.lens.makernotes.AdapterID;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sigma_X3F;
      strcpy(imgdata.lens.makernotes.Adapter, "MC-11");
    }

    else if ((imgdata.lens.makernotes.LensID > 0xEF00) && (imgdata.lens.makernotes.LensID < 0xFFFF) &&
             (imgdata.lens.makernotes.LensID != 0xFF00))
    {
      imgdata.lens.makernotes.AdapterID = 0xEF00;
      imgdata.lens.makernotes.LensID -= imgdata.lens.makernotes.AdapterID;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
    }
  }

  if ((id >= 286) && (id <= 293))
  {
    if (len <= 0x116)
      return;
    // "SLT-A65", "SLT-A77", "NEX-7", "NEX-VG20E",
    // "SLT-A37", "SLT-A57", "NEX-F3", "Lunar"
    parseSonyLensFeatures(SonySubstitution[buf[0x115]], SonySubstitution[buf[0x116]]);
  }
  else if (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens)
  {
    if (len <= 0x117)
      return;
    parseSonyLensFeatures(SonySubstitution[buf[0x116]], SonySubstitution[buf[0x117]]);
  }

  if ((id == 347) || (id == 350) || (id == 354) || (id == 357) || (id == 358) || (id == 360) || (id == 362))
  {
    if (len <= 0x8d)
      return;
    unsigned long long b88 = SonySubstitution[buf[0x88]];
    unsigned long long b89 = SonySubstitution[buf[0x89]];
    unsigned long long b8a = SonySubstitution[buf[0x8a]];
    unsigned long long b8b = SonySubstitution[buf[0x8b]];
    unsigned long long b8c = SonySubstitution[buf[0x8c]];
    unsigned long long b8d = SonySubstitution[buf[0x8d]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%06llx",
            (b88 << 40) + (b89 << 32) + (b8a << 24) + (b8b << 16) + (b8c << 8) + b8d);
  }
  else if (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Minolta_A)
  {
    if (len <= 0xf4)
      return;
    unsigned long long bf0 = SonySubstitution[buf[0xf0]];
    unsigned long long bf1 = SonySubstitution[buf[0xf1]];
    unsigned long long bf2 = SonySubstitution[buf[0xf2]];
    unsigned long long bf3 = SonySubstitution[buf[0xf3]];
    unsigned long long bf4 = SonySubstitution[buf[0xf4]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%05llx",
            (bf0 << 32) + (bf1 << 24) + (bf2 << 16) + (bf3 << 8) + bf4);
  }
  else if ((imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E) && (id != 288) && (id != 289) && (id != 290))
  {
    if (len <= 0x7f)
      return;
    unsigned b7c = SonySubstitution[buf[0x7c]];
    unsigned b7d = SonySubstitution[buf[0x7d]];
    unsigned b7e = SonySubstitution[buf[0x7e]];
    unsigned b7f = SonySubstitution[buf[0x7f]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%04x", (b7c << 24) + (b7d << 16) + (b7e << 8) + b7f);
  }

  if ((imgdata.makernotes.sony.ImageCount3_offset != 0xffff) &&
      (len >= (imgdata.makernotes.sony.ImageCount3_offset + 4)))
  {
    FORC4 s[c] = SonySubstitution[buf[imgdata.makernotes.sony.ImageCount3_offset + c]];
    imgdata.makernotes.sony.ImageCount3 = sget4(s);
  }

  if (id == 362)
  {
    for (c = 0; c < 6; c++)
    {
      imgdata.makernotes.sony.TimeStamp[c] = SonySubstitution[buf[0x0066 + c]];
    }
  }

  return;
}