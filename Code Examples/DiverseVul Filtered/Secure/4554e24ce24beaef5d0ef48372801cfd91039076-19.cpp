void CLASS initdata()
{
  tiff_flip = flip = filters = UINT_MAX; /* unknown */
  raw_height = raw_width = fuji_width = fuji_layout = cr2_slice[0] = 0;
  maximum = height = width = top_margin = left_margin = 0;
  cdesc[0] = desc[0] = artist[0] = make[0] = model[0] = model2[0] = 0;
  iso_speed = shutter = aperture = focal_len = unique_id = 0;
  tiff_nifds = 0;
  memset(tiff_ifd, 0, sizeof tiff_ifd);
  for (int i = 0; i < LIBRAW_IFD_MAXCOUNT; i++)
  {
    tiff_ifd[i].dng_color[0].illuminant = tiff_ifd[i].dng_color[1].illuminant = 0xffff;
    for (int c = 0; c < 4; c++)
      tiff_ifd[i].dng_levels.analogbalance[c] = 1.0f;
  }
  for (int i = 0; i < 0x10000; i++)
    curve[i] = i;
  memset(gpsdata, 0, sizeof gpsdata);
  memset(cblack, 0, sizeof cblack);
  memset(white, 0, sizeof white);
  memset(mask, 0, sizeof mask);
  thumb_offset = thumb_length = thumb_width = thumb_height = 0;
  load_raw = thumb_load_raw = 0;
  write_thumb = &CLASS jpeg_thumb;
  data_offset = meta_offset = meta_length = tiff_bps = tiff_compress = 0;
  kodak_cbpp = zero_after_ff = dng_version = load_flags = 0;
  timestamp = shot_order = tiff_samples = black = is_foveon = 0;
  mix_green = profile_length = data_error = zero_is_bad = 0;
  pixel_aspect = is_raw = raw_color = 1;
  tile_width = tile_length = 0;
}