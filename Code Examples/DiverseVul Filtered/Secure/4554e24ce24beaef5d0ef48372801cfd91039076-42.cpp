unsigned CLASS setCanonBodyFeatures(unsigned id)
{
  if (id == 0x03740000) // EOS M3
    id = 0x80000374;
  else if (id == 0x03840000) // EOS M10
    id = 0x80000384;
  else if (id == 0x03940000) // EOS M5
    id = 0x80000394;
  else if (id == 0x04070000) // EOS M6
    id = 0x80000407;
  else if (id == 0x03980000) // EOS M100
    id = 0x80000398;

  imgdata.lens.makernotes.CamID = id;
  if ((id == 0x80000001) || // 1D
      (id == 0x80000174) || // 1D2
      (id == 0x80000232) || // 1D2N
      (id == 0x80000169) || // 1D3
      (id == 0x80000281)    // 1D4
  )
  {
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSH;
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
  }
  else if ((id == 0x80000167) || // 1Ds
           (id == 0x80000188) || // 1Ds2
           (id == 0x80000215) || // 1Ds3
           (id == 0x80000269) || // 1DX
           (id == 0x80000328) || // 1DX2
           (id == 0x80000324) || // 1DC
           (id == 0x80000213) || // 5D
           (id == 0x80000218) || // 5D2
           (id == 0x80000285) || // 5D3
           (id == 0x80000349) || // 5D4
           (id == 0x80000382) || // 5DS
           (id == 0x80000401) || // 5DS R
           (id == 0x80000302)    // 6D
  )
  {
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_FF;
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
  }
  else if ((id == 0x80000331) || // M
           (id == 0x80000355) || // M2
           (id == 0x80000374) || // M3
           (id == 0x80000384) || // M10
           (id == 0x80000394) || // M5
           (id == 0x80000407) || // M6
           (id == 0x80000398)    // M100
  )
  {
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF_M;
  }
  else if ((id == 0x01140000) || // D30
           (id == 0x01668000) || // D60
           (id > 0x80000000))
  {
    imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Unknown;
  }
  else
  {
    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
  }

  return id;
}