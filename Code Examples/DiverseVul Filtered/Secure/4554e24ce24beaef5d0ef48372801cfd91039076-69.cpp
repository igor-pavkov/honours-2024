void CLASS parse_phase_one(int base)
{
  unsigned entries, tag, type, len, data, save, i, c;
  float romm_cam[3][3];
  char *cp;

  memset(&ph1, 0, sizeof ph1);
  fseek(ifp, base, SEEK_SET);
  order = get4() & 0xffff;
  if (get4() >> 8 != 0x526177)
    return; /* "Raw" */
  fseek(ifp, get4() + base, SEEK_SET);
  entries = get4();
  get4();
  while (entries--)
  {
    tag = get4();
    type = get4();
    len = get4();
    data = get4();
    save = ftell(ifp);
    fseek(ifp, base + data, SEEK_SET);
    switch (tag)
    {

#ifdef LIBRAW_LIBRARY_BUILD
    case 0x0102:
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
      if ((imgdata.shootinginfo.BodySerial[0] == 0x4c) && (imgdata.shootinginfo.BodySerial[1] == 0x49))
      {
        unique_id =
            (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) | (imgdata.shootinginfo.BodySerial[2] & 0x3f)) - 0x41;
      }
      else
      {
        unique_id =
            (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) | (imgdata.shootinginfo.BodySerial[1] & 0x3f)) - 0x41;
      }
      setPhaseOneFeatures(unique_id);
      break;
    case 0x0211:
      imgdata.other.SensorTemperature2 = int_to_float(data);
      break;
    case 0x0401:
      if (type == 4)
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      else
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      break;
    case 0x0403:
      if (type == 4)
        imgdata.lens.makernotes.CurFocal = int_to_float(data);
      else
        imgdata.lens.makernotes.CurFocal = getreal(type);
      break;
    case 0x0410:
      stmread(imgdata.lens.makernotes.body, len, ifp);
      break;
    case 0x0412:
      stmread(imgdata.lens.makernotes.Lens, len, ifp);
      break;
    case 0x0414:
      if (type == 4)
      {
        imgdata.lens.makernotes.MaxAp4CurFocal = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      }
      else
      {
        imgdata.lens.makernotes.MaxAp4CurFocal = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      }
      break;
    case 0x0415:
      if (type == 4)
      {
        imgdata.lens.makernotes.MinAp4CurFocal = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      }
      else
      {
        imgdata.lens.makernotes.MinAp4CurFocal = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      }
      break;
    case 0x0416:
      if (type == 4)
      {
        imgdata.lens.makernotes.MinFocal = int_to_float(data);
      }
      else
      {
        imgdata.lens.makernotes.MinFocal = getreal(type);
      }
      if (imgdata.lens.makernotes.MinFocal > 1000.0f)
      {
        imgdata.lens.makernotes.MinFocal = 0.0f;
      }
      break;
    case 0x0417:
      if (type == 4)
      {
        imgdata.lens.makernotes.MaxFocal = int_to_float(data);
      }
      else
      {
        imgdata.lens.makernotes.MaxFocal = getreal(type);
      }
      break;
#endif

    case 0x100:
      flip = "0653"[data & 3] - '0';
      break;
    case 0x106:
      for (i = 0; i < 9; i++)
#ifdef LIBRAW_LIBRARY_BUILD
        imgdata.color.P1_color[0].romm_cam[i] =
#endif
            ((float *)romm_cam)[i] = getreal(11);
      romm_coeff(romm_cam);
      break;
    case 0x107:
      FORC3 cam_mul[c] = getreal(11);
      break;
    case 0x108:
      raw_width = data;
      break;
    case 0x109:
      raw_height = data;
      break;
    case 0x10a:
      left_margin = data;
      break;
    case 0x10b:
      top_margin = data;
      break;
    case 0x10c:
      width = data;
      break;
    case 0x10d:
      height = data;
      break;
    case 0x10e:
      ph1.format = data;
      break;
    case 0x10f:
      data_offset = data + base;
      break;
    case 0x110:
      meta_offset = data + base;
      meta_length = len;
      break;
    case 0x112:
      ph1.key_off = save - 4;
      break;
    case 0x210:
      ph1.tag_210 = int_to_float(data);
#ifdef LIBRAW_LIBRARY_BUILD
      imgdata.other.SensorTemperature = ph1.tag_210;
#endif
      break;
    case 0x21a:
      ph1.tag_21a = data;
      break;
    case 0x21c:
      strip_offset = data + base;
      break;
    case 0x21d:
      ph1.t_black = data;
      break;
    case 0x222:
      ph1.split_col = data;
      break;
    case 0x223:
      ph1.black_col = data + base;
      break;
    case 0x224:
      ph1.split_row = data;
      break;
    case 0x225:
      ph1.black_row = data + base;
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 0x226:
      for (i = 0; i < 9; i++)
        imgdata.color.P1_color[1].romm_cam[i] = getreal(11);
      break;
#endif
    case 0x301:
      model[63] = 0;
      fread(model, 1, 63, ifp);
      if ((cp = strstr(model, " camera")))
        *cp = 0;
    }
    fseek(ifp, save, SEEK_SET);
  }

#ifdef LIBRAW_LIBRARY_BUILD
  if (!imgdata.lens.makernotes.body[0] && !imgdata.shootinginfo.BodySerial[0])
  {
    fseek(ifp, meta_offset, SEEK_SET);
    order = get2();
    fseek(ifp, 6, SEEK_CUR);
    fseek(ifp, meta_offset + get4(), SEEK_SET);
    entries = get4();
    get4();
    while (entries--)
    {
      tag = get4();
      len = get4();
      data = get4();
      save = ftell(ifp);
      fseek(ifp, meta_offset + data, SEEK_SET);
      if (tag == 0x0407)
      {
        stmread(imgdata.shootinginfo.BodySerial, len, ifp);
        if ((imgdata.shootinginfo.BodySerial[0] == 0x4c) && (imgdata.shootinginfo.BodySerial[1] == 0x49))
        {
          unique_id =
              (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) | (imgdata.shootinginfo.BodySerial[2] & 0x3f)) - 0x41;
        }
        else
        {
          unique_id =
              (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) | (imgdata.shootinginfo.BodySerial[1] & 0x3f)) - 0x41;
        }
        setPhaseOneFeatures(unique_id);
      }
      fseek(ifp, save, SEEK_SET);
    }
  }
#endif

  load_raw = ph1.format < 3 ? &CLASS phase_one_load_raw : &CLASS phase_one_load_raw_c;
  maximum = 0xffff;
  strcpy(make, "Phase One");
  if (model[0])
    return;
  switch (raw_height)
  {
  case 2060:
    strcpy(model, "LightPhase");
    break;
  case 2682:
    strcpy(model, "H 10");
    break;
  case 4128:
    strcpy(model, "H 20");
    break;
  case 5488:
    strcpy(model, "H 25");
    break;
  }
}