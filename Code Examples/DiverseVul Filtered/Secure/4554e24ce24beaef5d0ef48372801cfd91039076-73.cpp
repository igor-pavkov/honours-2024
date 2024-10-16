void CLASS adobe_copy_pixel(unsigned row, unsigned col, ushort **rp)
{
  int c;

  if (tiff_samples == 2 && shot_select)
    (*rp)++;
  if (raw_image)
  {
    if (row < raw_height && col < raw_width)
      RAW(row, col) = curve[**rp];
    *rp += tiff_samples;
  }
  else
  {
#ifdef LIBRAW_LIBRARY_BUILD
    if (row < raw_height && col < raw_width)
      FORC(tiff_samples)
    image[row * raw_width + col][c] = curve[(*rp)[c]];
    *rp += tiff_samples;
#else
    if (row < height && col < width)
      FORC(tiff_samples)
    image[row * width + col][c] = curve[(*rp)[c]];
    *rp += tiff_samples;
#endif
  }
  if (tiff_samples == 2 && shot_select)
    (*rp)--;
}