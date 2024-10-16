void CLASS pre_interpolate()
{
  ushort(*img)[4];
  int row, col, c;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_PRE_INTERPOLATE, 0, 2);
#endif
  if (shrink)
  {
    if (half_size)
    {
      height = iheight;
      width = iwidth;
      if (filters == 9)
      {
        for (row = 0; row < 3; row++)
          for (col = 1; col < 4; col++)
            if (!(image[row * width + col][0] | image[row * width + col][2]))
              goto break2;
      break2:
        for (; row < height; row += 3)
          for (col = (col - 1) % 3 + 1; col < width - 1; col += 3)
          {
            img = image + row * width + col;
            for (c = 0; c < 3; c += 2)
              img[0][c] = (img[-1][c] + img[1][c]) >> 1;
          }
      }
    }
    else
    {
      img = (ushort(*)[4])calloc(height, width * sizeof *img);
      merror(img, "pre_interpolate()");
      for (row = 0; row < height; row++)
        for (col = 0; col < width; col++)
        {
          c = fcol(row, col);
          img[row * width + col][c] = image[(row >> 1) * iwidth + (col >> 1)][c];
        }
      free(image);
      image = img;
      shrink = 0;
    }
  }
  if (filters > 1000 && colors == 3)
  {
    mix_green = four_color_rgb ^ half_size;
    if (four_color_rgb | half_size)
      colors++;
    else
    {
      for (row = FC(1, 0) >> 1; row < height; row += 2)
        for (col = FC(row, 1) & 1; col < width; col += 2)
          image[row * width + col][1] = image[row * width + col][3];
      filters &= ~((filters & 0x55555555U) << 1);
    }
  }
  if (half_size)
    filters = 0;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_PRE_INTERPOLATE, 1, 2);
#endif
}