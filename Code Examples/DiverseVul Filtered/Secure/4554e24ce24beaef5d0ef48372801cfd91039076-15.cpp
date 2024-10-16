void CLASS processNikonLensData(uchar *LensData, unsigned len)
{
  ushort i;
  if (!(imgdata.lens.nikon.NikonLensType & 0x01))
  {
    imgdata.lens.makernotes.LensFeatures_pre[0] = 'A';
    imgdata.lens.makernotes.LensFeatures_pre[1] = 'F';
  }
  else
  {
    imgdata.lens.makernotes.LensFeatures_pre[0] = 'M';
    imgdata.lens.makernotes.LensFeatures_pre[1] = 'F';
  }

  if (imgdata.lens.nikon.NikonLensType & 0x02)
  {
    if (imgdata.lens.nikon.NikonLensType & 0x04)
      imgdata.lens.makernotes.LensFeatures_suf[0] = 'G';
    else
      imgdata.lens.makernotes.LensFeatures_suf[0] = 'D';
    imgdata.lens.makernotes.LensFeatures_suf[1] = ' ';
  }

  if (imgdata.lens.nikon.NikonLensType & 0x08)
  {
    imgdata.lens.makernotes.LensFeatures_suf[2] = 'V';
    imgdata.lens.makernotes.LensFeatures_suf[3] = 'R';
  }

  if (imgdata.lens.nikon.NikonLensType & 0x10)
  {
    imgdata.lens.makernotes.LensMount = imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Nikon_CX;
    imgdata.lens.makernotes.CameraFormat = imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_1INCH;
  }
  else
    imgdata.lens.makernotes.LensMount = imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Nikon_F;

  if (imgdata.lens.nikon.NikonLensType & 0x20)
  {
    strcpy(imgdata.lens.makernotes.Adapter, "FT-1");
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Nikon_F;
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Nikon_CX;
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_1INCH;
  }

  imgdata.lens.nikon.NikonLensType = imgdata.lens.nikon.NikonLensType & 0xdf;

  if (len < 20)
  {
    switch (len)
    {
    case 9:
      i = 2;
      break;
    case 15:
      i = 7;
      break;
    case 16:
      i = 8;
      break;
    }
    imgdata.lens.nikon.NikonLensIDNumber = LensData[i];
    imgdata.lens.nikon.NikonLensFStops = LensData[i + 1];
    imgdata.lens.makernotes.LensFStops = (float)imgdata.lens.nikon.NikonLensFStops / 12.0f;
    if (fabsf(imgdata.lens.makernotes.MinFocal) < 1.1f)
    {
      if ((imgdata.lens.nikon.NikonLensType ^ (uchar)0x01) || LensData[i + 2])
        imgdata.lens.makernotes.MinFocal = 5.0f * libraw_powf64l(2.0f, (float)LensData[i + 2] / 24.0f);
      if ((imgdata.lens.nikon.NikonLensType ^ (uchar)0x01) || LensData[i + 3])
        imgdata.lens.makernotes.MaxFocal = 5.0f * libraw_powf64l(2.0f, (float)LensData[i + 3] / 24.0f);
      if ((imgdata.lens.nikon.NikonLensType ^ (uchar)0x01) || LensData[i + 4])
        imgdata.lens.makernotes.MaxAp4MinFocal = libraw_powf64l(2.0f, (float)LensData[i + 4] / 24.0f);
      if ((imgdata.lens.nikon.NikonLensType ^ (uchar)0x01) || LensData[i + 5])
        imgdata.lens.makernotes.MaxAp4MaxFocal = libraw_powf64l(2.0f, (float)LensData[i + 5] / 24.0f);
    }
    imgdata.lens.nikon.NikonMCUVersion = LensData[i + 6];
    if (i != 2)
    {
      if ((LensData[i - 1]) && (fabsf(imgdata.lens.makernotes.CurFocal) < 1.1f))
        imgdata.lens.makernotes.CurFocal = 5.0f * libraw_powf64l(2.0f, (float)LensData[i - 1] / 24.0f);
      if (LensData[i + 7])
        imgdata.lens.nikon.NikonEffectiveMaxAp = libraw_powf64l(2.0f, (float)LensData[i + 7] / 24.0f);
    }
    imgdata.lens.makernotes.LensID =
        (unsigned long long)LensData[i] << 56 | (unsigned long long)LensData[i + 1] << 48 |
        (unsigned long long)LensData[i + 2] << 40 | (unsigned long long)LensData[i + 3] << 32 |
        (unsigned long long)LensData[i + 4] << 24 | (unsigned long long)LensData[i + 5] << 16 |
        (unsigned long long)LensData[i + 6] << 8 | (unsigned long long)imgdata.lens.nikon.NikonLensType;
  }
  else if ((len == 459) || (len == 590))
  {
    memcpy(imgdata.lens.makernotes.Lens, LensData + 390, 64);
  }
  else if (len == 509)
  {
    memcpy(imgdata.lens.makernotes.Lens, LensData + 391, 64);
  }
  else if (len == 879)
  {
    memcpy(imgdata.lens.makernotes.Lens, LensData + 680, 64);
  }
  return;
}