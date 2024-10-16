void CLASS parse_ciff (int offset, int length, int depth)
{
  int tboff, nrecs, c, type, len, save, wbi=-1;
  ushort key[] = { 0x410, 0x45f3 };

  fseek (ifp, offset+length-4, SEEK_SET);
  tboff = get4() + offset;
  fseek (ifp, tboff, SEEK_SET);
  nrecs = get2();
  if ((nrecs | depth) > 127) return;
  while (nrecs--) {
    type = get2();
    len  = get4();
    save = ftell(ifp) + 4;
    fseek (ifp, offset+get4(), SEEK_SET);
    if ((((type >> 8) + 8) | 8) == 0x38) {
      parse_ciff (ftell(ifp), len, depth+1); /* Parse a sub-table */
    }
#ifdef LIBRAW_LIBRARY_BUILD
    if (type == 0x3004) parse_ciff (ftell(ifp), len, depth+1);
#endif
    if (type == 0x0810)
      fread (artist, 64, 1, ifp);
    if (type == 0x080a) {
      fread (make, 64, 1, ifp);
      fseek (ifp, strbuflen(make) - 63, SEEK_CUR);
      fread (model, 64, 1, ifp);
    }
    if (type == 0x1810) {
      width = get4();
      height = get4();
      pixel_aspect = int_to_float(get4());
      flip = get4();
    }
    if (type == 0x1835)			/* Get the decoder table */
      tiff_compress = get4();
    if (type == 0x2007) {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (type == 0x1818) {
      shutter = libraw_powf64(2.0f, -int_to_float((get4(),get4())));
      aperture = libraw_powf64(2.0f, int_to_float(get4())/2);
#ifdef LIBRAW_LIBRARY_BUILD
      imgdata.lens.makernotes.CurAp = aperture;
#endif
    }
    if (type == 0x102a) {
//      iso_speed = pow (2.0, (get4(),get2())/32.0 - 4) * 50;
      iso_speed = libraw_powf64(2.0f, ((get2(),get2()) + get2())/32.0f - 5.0f) * 100.0f;
#ifdef LIBRAW_LIBRARY_BUILD
      aperture  = _CanonConvertAperture((get2(),get2()));
      imgdata.lens.makernotes.CurAp = aperture;
#else
      aperture  = libraw_powf64(2.0, (get2(),(short)get2())/64.0);
#endif
      shutter   = libraw_powf64(2.0,-((short)get2())/32.0);
      wbi = (get2(),get2());
      if (wbi > 17) wbi = 0;
      fseek (ifp, 32, SEEK_CUR);
      if (shutter > 1e6) shutter = get2()/10.0;
    }
    if (type == 0x102c) {
      if (get2() > 512) {		/* Pro90, G1 */
	fseek (ifp, 118, SEEK_CUR);
	FORC4 cam_mul[c ^ 2] = get2();
      } else {				/* G2, S30, S40 */
	fseek (ifp, 98, SEEK_CUR);
	FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get2();
      }
    }
#ifdef LIBRAW_LIBRARY_BUILD
    if (type == 0x10a9)
      {
	INT64 o = ftell(ifp);
	fseek (ifp, (0x5<<1), SEEK_CUR);
	Canon_WBpresets(0,0);
	fseek(ifp,o,SEEK_SET);
      }
    if (type == 0x102d)
      {
	INT64 o = ftell(ifp);
	Canon_CameraSettings();
	fseek(ifp,o,SEEK_SET);
      }
    if (type == 0x580b)
      {
        if (strcmp(model,"Canon EOS D30")) sprintf(imgdata.shootinginfo.BodySerial, "%d", len);
        else sprintf(imgdata.shootinginfo.BodySerial, "%0x-%05d", len>>16, len&0xffff);
      }
#endif
    if (type == 0x0032) {
      if (len == 768) {			/* EOS D30 */
	fseek (ifp, 72, SEEK_CUR);
        FORC4
        {
          ushort q = get2();
          cam_mul[c ^ (c >> 1)] = q? 1024.0 / get2() : 1024;
        }
	if (!wbi) cam_mul[0] = -1;	/* use my auto white balance */
      } else if (!cam_mul[0]) {
	if (get2() == key[0])		/* Pro1, G6, S60, S70 */
	  c = (strstr(model,"Pro1") ?
	      "012346000000000000":"01345:000000006008")[LIM(0,wbi,17)]-'0'+ 2;
	else {				/* G3, G5, S45, S50 */
	  c = "023457000000006000"[LIM(0,wbi,17)]-'0';
	  key[0] = key[1] = 0;
	}
	fseek (ifp, 78 + c*8, SEEK_CUR);
	FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get2() ^ key[c & 1];
	if (!wbi) cam_mul[0] = -1;
      }
    }
    if (type == 0x10a9) {		/* D60, 10D, 300D, and clones */
      if (len > 66) wbi = "0134567028"[LIM(0,wbi,9)]-'0';
      fseek (ifp, 2 + wbi*8, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
    }
    if (type == 0x1030 && wbi>=0 && (0x18040 >> wbi & 1))
      ciff_block_1030();		/* all that don't have 0x10a9 */
    if (type == 0x1031) {
      raw_width = (get2(),get2());
      raw_height = get2();
    }
    if (type == 0x501c) {
      iso_speed = len & 0xffff;
    }
    if (type == 0x5029) {
#ifdef LIBRAW_LIBRARY_BUILD
      imgdata.lens.makernotes.CurFocal  = len >> 16;
      imgdata.lens.makernotes.FocalType = len & 0xffff;
      if (imgdata.lens.makernotes.FocalType == 2) {
        imgdata.lens.makernotes.CanonFocalUnits = 32;
	if(imgdata.lens.makernotes.CanonFocalUnits>1)
	  imgdata.lens.makernotes.CurFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
      }
      focal_len = imgdata.lens.makernotes.CurFocal;
#else
      focal_len = len >> 16;
      if ((len & 0xffff) == 2) focal_len /= 32;
#endif
    }
    if (type == 0x5813) flash_used = int_to_float(len);
    if (type == 0x5814) canon_ev   = int_to_float(len);
    if (type == 0x5817) shot_order = len;
    if (type == 0x5834)
      {
         unique_id  = len;
#ifdef LIBRAW_LIBRARY_BUILD
         setCanonBodyFeatures(unique_id);
#endif
      }
    if (type == 0x580e) timestamp  = len;
    if (type == 0x180e) timestamp  = get4();
#ifdef LOCALTIME
    if ((type | 0x4000) == 0x580e)
      timestamp = mktime (gmtime (&timestamp));
#endif
    fseek (ifp, save, SEEK_SET);
  }
}