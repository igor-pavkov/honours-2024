void CLASS imacon_full_load_raw()
{
#ifdef LIBRAW_LIBRARY_BUILD
  if (!image)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
#endif
  int row, col;

#ifdef LIBRAW_LIBRARY_BUILD
  unsigned short *buf = (unsigned short *)malloc(width * 3 * sizeof(unsigned short));
  merror(buf, "imacon_full_load_raw");
#endif

  for (row = 0; row < height; row++)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
    read_shorts(buf, width * 3);
    unsigned short(*rowp)[4] = &image[row * width];
    for (col = 0; col < width; col++)
    {
      rowp[col][0] = buf[col * 3];
      rowp[col][1] = buf[col * 3 + 1];
      rowp[col][2] = buf[col * 3 + 2];
      rowp[col][3] = 0;
    }
#else
    for (col = 0; col < width; col++)
      read_shorts(image[row * width + col], 3);
#endif
  }
#ifdef LIBRAW_LIBRARY_BUILD
  free(buf);
#endif
}