void CLASS apply_tiff()
{
  int max_samp = 0, ties = 0, raw = -1, thm = -1, i;
  unsigned long long ns, os;
  struct jhead jh;

  thumb_misc = 16;
  if (thumb_offset)
  {
    fseek(ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start(&jh, 1))
    {
      if ((unsigned)jh.bits < 17 && (unsigned)jh.wide < 0x10000 && (unsigned)jh.high < 0x10000)
      {
        thumb_misc = jh.bits;
        thumb_width = jh.wide;
        thumb_height = jh.high;
      }
    }
  }
  for (i = tiff_nifds; i--;)
  {
    if (tiff_ifd[i].t_shutter)
      shutter = tiff_ifd[i].t_shutter;
    tiff_ifd[i].t_shutter = shutter;
  }
  for (i = 0; i < tiff_nifds; i++)
  {
    if( tiff_ifd[i].t_width < 1 ||  tiff_ifd[i].t_width > 65535
       || tiff_ifd[i].t_height < 1 || tiff_ifd[i].t_height > 65535)
          continue; /* wrong image dimensions */
    if (max_samp < tiff_ifd[i].samples)
      max_samp = tiff_ifd[i].samples;
    if (max_samp > 3)
      max_samp = 3;
    os = raw_width * raw_height;
    ns = tiff_ifd[i].t_width * tiff_ifd[i].t_height;
    if (tiff_bps)
    {
      os *= tiff_bps;
      ns *= tiff_ifd[i].bps;
    }
    if ((tiff_ifd[i].comp != 6 || tiff_ifd[i].samples != 3) &&
        unsigned(tiff_ifd[i].t_width | tiff_ifd[i].t_height) < 0x10000 && (unsigned)tiff_ifd[i].bps < 33 &&
        (unsigned)tiff_ifd[i].samples < 13 && ns && ((ns > os && (ties = 1)) || (ns == os && shot_select == ties++)))
    {
      raw_width = tiff_ifd[i].t_width;
      raw_height = tiff_ifd[i].t_height;
      tiff_bps = tiff_ifd[i].bps;
      tiff_compress = tiff_ifd[i].comp;
      data_offset = tiff_ifd[i].offset;
#ifdef LIBRAW_LIBRARY_BUILD
      data_size = tiff_ifd[i].bytes;
#endif
      tiff_flip = tiff_ifd[i].t_flip;
      tiff_samples = tiff_ifd[i].samples;
      tile_width = tiff_ifd[i].t_tile_width;
      tile_length = tiff_ifd[i].t_tile_length;
      shutter = tiff_ifd[i].t_shutter;
      raw = i;
    }
  }
  if (is_raw == 1 && ties)
    is_raw = ties;
  if (!tile_width)
    tile_width = INT_MAX;
  if (!tile_length)
    tile_length = INT_MAX;
  for (i = tiff_nifds; i--;)
    if (tiff_ifd[i].t_flip)
      tiff_flip = tiff_ifd[i].t_flip;
  if (raw >= 0 && !load_raw)
    switch (tiff_compress)
    {
    case 32767:
#ifdef LIBRAW_LIBRARY_BUILD
      if (!dng_version && INT64(tiff_ifd[raw].bytes) == INT64(raw_width) * INT64(raw_height))
#else
      if (tiff_ifd[raw].bytes == raw_width * raw_height)
#endif
      {
        tiff_bps = 14;
        load_raw = &CLASS sony_arw2_load_raw;
        break;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      if (!dng_version && !strncasecmp(make, "Sony", 4) && INT64(tiff_ifd[raw].bytes) == INT64(raw_width) * INT64(raw_height) * 2ULL)
#else
      if (!strncasecmp(make, "Sony", 4) && tiff_ifd[raw].bytes == raw_width * raw_height * 2)
#endif
      {
        tiff_bps = 14;
        load_raw = &CLASS unpacked_load_raw;
        break;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      if (INT64(tiff_ifd[raw].bytes) * 8ULL != INT64(raw_width) * INT64(raw_height) * INT64(tiff_bps))
#else
      if (tiff_ifd[raw].bytes * 8 != raw_width * raw_height * tiff_bps)
#endif
      {
        raw_height += 8;
        load_raw = &CLASS sony_arw_load_raw;
        break;
      }
      load_flags = 79;
    case 32769:
      load_flags++;
    case 32770:
    case 32773:
      goto slr;
    case 0:
    case 1:
#ifdef LIBRAW_LIBRARY_BUILD
      // Sony 14-bit uncompressed
      if (!dng_version && !strncasecmp(make, "Sony", 4) && INT64(tiff_ifd[raw].bytes) == INT64(raw_width) * INT64(raw_height) * 2ULL)
      {
        tiff_bps = 14;
        load_raw = &CLASS unpacked_load_raw;
        break;
      }
      if (!dng_version && !strncasecmp(make, "Sony", 4) && tiff_ifd[raw].samples == 4 &&
          INT64(tiff_ifd[raw].bytes) == INT64(raw_width) * INT64(raw_height) * 8ULL) // Sony ARQ
      {
        tiff_bps = 14;
        tiff_samples = 4;
        load_raw = &CLASS sony_arq_load_raw;
        filters = 0;
        strcpy(cdesc, "RGBG");
        break;
      }
      if (!strncasecmp(make, "Nikon", 5) && !strncmp(software, "Nikon Scan", 10))
      {
        load_raw = &CLASS nikon_coolscan_load_raw;
        raw_color = 1;
        filters = 0;
        break;
      }
      if (!strncmp(make, "OLYMPUS", 7) && INT64(tiff_ifd[raw].bytes) * 2ULL == INT64(raw_width) * INT64(raw_height) * 3ULL)
#else 
      if (!strncmp(make, "OLYMPUS", 7) && tiff_ifd[raw].bytes * 2 == raw_width * raw_height * 3)
#endif
        load_flags = 24;
#ifdef LIBRAW_LIBRARY_BUILD
      if (!dng_version && INT64(tiff_ifd[raw].bytes) * 5ULL == INT64(raw_width) * INT64(raw_height) * 8ULL)
#else
      if (tiff_ifd[raw].bytes * 5 == raw_width * raw_height * 8)
#endif
      {
        load_flags = 81;
        tiff_bps = 12;
      }
    slr:
      switch (tiff_bps)
      {
      case 8:
        load_raw = &CLASS eight_bit_load_raw;
        break;
      case 12:
        if (tiff_ifd[raw].phint == 2)
          load_flags = 6;
        load_raw = &CLASS packed_load_raw;
        break;
      case 14:
        load_flags = 0;
      case 16:
        load_raw = &CLASS unpacked_load_raw;
#ifdef LIBRAW_LIBRARY_BUILD
        if (!strncmp(make, "OLYMPUS", 7) && INT64(tiff_ifd[raw].bytes) * 7ULL > INT64(raw_width) * INT64(raw_height))
#else
        if (!strncmp(make, "OLYMPUS", 7) && tiff_ifd[raw].bytes * 7 > raw_width * raw_height)
#endif
          load_raw = &CLASS olympus_load_raw;
      }
      break;
    case 6:
    case 7:
    case 99:
      load_raw = &CLASS lossless_jpeg_load_raw;
      break;
    case 262:
      load_raw = &CLASS kodak_262_load_raw;
      break;
    case 34713:
#ifdef LIBRAW_LIBRARY_BUILD
      if ((INT64(raw_width) + 9ULL) / 10ULL * 16ULL * INT64(raw_height) == INT64(tiff_ifd[raw].bytes))
#else
      if ((raw_width + 9) / 10 * 16 * raw_height == tiff_ifd[raw].bytes)
#endif
      {
        load_raw = &CLASS packed_load_raw;
        load_flags = 1;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      else if (INT64(raw_width) * INT64(raw_height) * 3ULL == INT64(tiff_ifd[raw].bytes) * 2ULL)
#else
      else if (raw_width * raw_height * 3 == tiff_ifd[raw].bytes * 2)
#endif
      {
        load_raw = &CLASS packed_load_raw;
        if (model[0] == 'N')
          load_flags = 80;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      else if (INT64(raw_width) * INT64(raw_height) * 3ULL == INT64(tiff_ifd[raw].bytes))
#else
      else if (raw_width * raw_height * 3 == tiff_ifd[raw].bytes)
#endif
      {
        load_raw = &CLASS nikon_yuv_load_raw;
        gamma_curve(1 / 2.4, 12.92, 1, 4095);
        memset(cblack, 0, sizeof cblack);
        filters = 0;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      else if (INT64(raw_width) * INT64(raw_height) * 2ULL == INT64(tiff_ifd[raw].bytes))
#else
      else if (raw_width * raw_height * 2 == tiff_ifd[raw].bytes)
#endif
      {
        load_raw = &CLASS unpacked_load_raw;
        load_flags = 4;
        order = 0x4d4d;
      }
      else
#ifdef LIBRAW_LIBRARY_BUILD
          if (INT64(raw_width) * INT64(raw_height) * 3ULL == INT64(tiff_ifd[raw].bytes) * 2ULL)
      {
        load_raw = &CLASS packed_load_raw;
        load_flags = 80;
      }
      else if (tiff_ifd[raw].rows_per_strip && tiff_ifd[raw].strip_offsets_count &&
               tiff_ifd[raw].strip_offsets_count == tiff_ifd[raw].strip_byte_counts_count)
      {
        int fit = 1;
        for (int i = 0; i < tiff_ifd[raw].strip_byte_counts_count - 1; i++) // all but last
          if (INT64(tiff_ifd[raw].strip_byte_counts[i]) * 2ULL != INT64(tiff_ifd[raw].rows_per_strip) * INT64(raw_width) * 3ULL)
          {
            fit = 0;
            break;
          }
        if (fit)
          load_raw = &CLASS nikon_load_striped_packed_raw;
        else
          load_raw = &CLASS nikon_load_raw; // fallback
      }
      else
#endif
        load_raw = &CLASS nikon_load_raw;
      break;
    case 65535:
      load_raw = &CLASS pentax_load_raw;
      break;
    case 65000:
      switch (tiff_ifd[raw].phint)
      {
      case 2:
        load_raw = &CLASS kodak_rgb_load_raw;
        filters = 0;
        break;
      case 6:
        load_raw = &CLASS kodak_ycbcr_load_raw;
        filters = 0;
        break;
      case 32803:
        load_raw = &CLASS kodak_65000_load_raw;
      }
    case 32867:
    case 34892:
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 8:
      break;
#endif
    default:
      is_raw = 0;
    }
  if (!dng_version)
    if (((tiff_samples == 3 && tiff_ifd[raw].bytes && tiff_bps != 14 &&
      !(tiff_bps == 16 && !strncmp(make, "Leaf", 4)) && // Allow Leaf/16bit/3color files
    (tiff_compress & -16) != 32768) ||
         (tiff_bps == 8 && strncmp(make, "Phase", 5) && strncmp(make, "Leaf", 4) && !strcasestr(make, "Kodak") &&
          !strstr(model2, "DEBUG RAW"))) &&
        strncmp(software, "Nikon Scan", 10))
      is_raw = 0;

  for (i = 0; i < tiff_nifds; i++)
    if (i != raw &&
        (tiff_ifd[i].samples == max_samp || (tiff_ifd[i].comp == 7 && tiff_ifd[i].samples == 1)) /* Allow 1-bps JPEGs */
        && tiff_ifd[i].bps > 0 && tiff_ifd[i].bps < 33 && tiff_ifd[i].phint != 32803 && tiff_ifd[i].phint != 34892 &&
        unsigned(tiff_ifd[i].t_width | tiff_ifd[i].t_height) < 0x10000 &&
        tiff_ifd[i].t_width * tiff_ifd[i].t_height / (SQR(tiff_ifd[i].bps) + 1) >
            thumb_width * thumb_height / (SQR(thumb_misc) + 1) &&
        tiff_ifd[i].comp != 34892)
    {
      thumb_width = tiff_ifd[i].t_width;
      thumb_height = tiff_ifd[i].t_height;
      thumb_offset = tiff_ifd[i].offset;
      thumb_length = tiff_ifd[i].bytes;
      thumb_misc = tiff_ifd[i].bps;
      thm = i;
    }
  if (thm >= 0)
  {
    thumb_misc |= tiff_ifd[thm].samples << 5;
    switch (tiff_ifd[thm].comp)
    {
    case 0:
      write_thumb = &CLASS layer_thumb;
      break;
    case 1:
      if (tiff_ifd[thm].bps <= 8)
        write_thumb = &CLASS ppm_thumb;
      else if (!strncmp(make, "Imacon", 6))
        write_thumb = &CLASS ppm16_thumb;
      else
        thumb_load_raw = &CLASS kodak_thumb_load_raw;
      break;
    case 65000:
      thumb_load_raw = tiff_ifd[thm].phint == 6 ? &CLASS kodak_ycbcr_load_raw : &CLASS kodak_rgb_load_raw;
    }
  }
}