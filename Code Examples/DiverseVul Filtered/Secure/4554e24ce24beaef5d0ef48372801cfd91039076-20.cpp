void CLASS setOlympusBodyFeatures(unsigned long long id)
{
  imgdata.lens.makernotes.CamID = id;
  if (id == 0x5330303638ULL)
  {
    strcpy(model, "E-M10MarkIII");
  }
  if ((id == 0x4434303430ULL) || // E-1
      (id == 0x4434303431ULL) || // E-300
      ((id & 0x00ffff0000ULL) == 0x0030300000ULL))
  {
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_FT;
    if ((id == 0x4434303430ULL) ||                              // E-1
        (id == 0x4434303431ULL) ||                              // E-330
        ((id >= 0x5330303033ULL) && (id <= 0x5330303138ULL)) || // E-330 to E-520
        (id == 0x5330303233ULL) ||                              // E-620
        (id == 0x5330303239ULL) ||                              // E-450
        (id == 0x5330303330ULL) ||                              // E-600
        (id == 0x5330303333ULL))                                // E-5
    {
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FT;
    }
    else
    {
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_mFT;
    }
  }
  else
  {
    imgdata.lens.makernotes.LensMount = imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
  }
  return;
}