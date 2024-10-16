void CLASS setSonyBodyFeatures(unsigned id)
{
  ushort idx;
  static const struct
  {
    ushort scf[8];
    /*
    scf[0] camera id
    scf[1] camera format
    scf[2] camera mount: Minolta A, Sony E, fixed,
    scf[3] camera type: DSLR, NEX, SLT, ILCE, ILCA, DSC
    scf[4] lens mount
    scf[5] tag 0x2010 group (0 if not used)
    scf[6] offset of Sony ISO in 0x2010 table, 0xffff if not valid
    scf[7] offset of ImageCount3 in 0x9050 table, 0xffff if not valid
    */
  } SonyCamFeatures[] = {
      {256, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {257, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {258, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {259, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {260, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {261, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {262, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {263, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {264, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {265, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {266, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {267, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {268, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {269, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {270, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {271, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {272, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {273, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {274, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {275, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {276, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {277, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {278, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff},
      {279, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff},
      {280, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff},
      {281, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff},
      {282, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {283, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff},
      {284, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff},
      {285, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff},
      {286, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2, 0x1218, 0x01bd},
      {287, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2, 0x1218, 0x01bd},
      {288, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 1, 0x113e, 0x01bd},
      {289, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2, 0x1218, 0x01bd},
      {290, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2, 0x1218, 0x01bd},
      {291, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3, 0x11f4, 0x01bd},
      {292, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3, 0x11f4, 0x01bd},
      {293, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 3, 0x11f4, 0x01bd},
      {294, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5, 0x1254, 0x01aa},
      {295, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa},
      {296, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa},
      {297, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1254, 0xffff},
      {298, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff},
      {299, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa},
      {300, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa},
      {301, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {302, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 5, 0x1280, 0x01aa},
      {303, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5, 0x1280, 0x01aa},
      {304, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {305, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1280, 0x01aa},
      {306, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff},
      {307, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa},
      {308, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 6, 0x113c, 0xffff},
      {309, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff},
      {310, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff},
      {311, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff},
      {312, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff},
      {313, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01aa},
      {314, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {315, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {316, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {317, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff},
      {318, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff},
      {319, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 7, 0x0344, 0x01a0},
      {320, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {321, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {322, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {323, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {324, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {325, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {326, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {327, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {328, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {329, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {330, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {331, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {332, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {333, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {334, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {335, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {336, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {337, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {338, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {339, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01a0},
      {340, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff},
      {341, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {342, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {343, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {344, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {345, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {346, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01a0},
      {347, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cb},
      {348, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {349, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {350, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cb},
      {351, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {352, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {353, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 7, 0x0344, 0x01a0},
      {354, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 8, 0x0346, 0x01cd},
      {355, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {356, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {357, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cd},
      {358, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9, 0x0320, 0x019f},
      {359, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {360, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cd},
      {361, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {362, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9, 0x0320, 0x019f},
      {363, 0, 0, 0, 0, 0, 0xffff, 0xffff},
      {364, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff},
      {365, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff},
  };
  imgdata.lens.makernotes.CamID = id;

  if (id == 2)
  {
    imgdata.lens.makernotes.CameraMount = imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
    imgdata.makernotes.sony.SonyCameraType = LIBRAW_SONY_DSC;
    imgdata.makernotes.sony.group2010 = 0;
    imgdata.makernotes.sony.real_iso_offset = 0xffff;
    imgdata.makernotes.sony.ImageCount3_offset = 0xffff;
    return;
  }
  else
    idx = id - 256;

  if ((idx >= 0) && (idx < sizeof SonyCamFeatures / sizeof *SonyCamFeatures))
  {
    if (!SonyCamFeatures[idx].scf[2])
      return;
    imgdata.lens.makernotes.CameraFormat = SonyCamFeatures[idx].scf[1];
    imgdata.lens.makernotes.CameraMount = SonyCamFeatures[idx].scf[2];
    imgdata.makernotes.sony.SonyCameraType = SonyCamFeatures[idx].scf[3];
    if (SonyCamFeatures[idx].scf[4])
      imgdata.lens.makernotes.LensMount = SonyCamFeatures[idx].scf[4];
    imgdata.makernotes.sony.group2010 = SonyCamFeatures[idx].scf[5];
    imgdata.makernotes.sony.real_iso_offset = SonyCamFeatures[idx].scf[6];
    imgdata.makernotes.sony.ImageCount3_offset = SonyCamFeatures[idx].scf[7];
  }

  char *sbstr = strstr(software, " v");
  if (sbstr != NULL)
  {
    sbstr += 2;
    imgdata.makernotes.sony.firmware = atof(sbstr);

    if ((id == 306) || (id == 311))
    {
      if (imgdata.makernotes.sony.firmware < 1.2f)
        imgdata.makernotes.sony.ImageCount3_offset = 0x01aa;
      else
        imgdata.makernotes.sony.ImageCount3_offset = 0x01c0;
    }
    else if (id == 312)
    {
      if (imgdata.makernotes.sony.firmware < 2.0f)
        imgdata.makernotes.sony.ImageCount3_offset = 0x01aa;
      else
        imgdata.makernotes.sony.ImageCount3_offset = 0x01c0;
    }
    else if ((id == 318) || (id == 340))
    {
      if (imgdata.makernotes.sony.firmware < 1.2f)
        imgdata.makernotes.sony.ImageCount3_offset = 0x01a0;
      else
        imgdata.makernotes.sony.ImageCount3_offset = 0x01b6;
    }
  }
}