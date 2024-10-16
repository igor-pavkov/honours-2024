static int jpeg_extract(Image *ifile, Image *ofile)
{
  unsigned int marker;
  unsigned int done = 0;

  if (jpeg_skip_1(ifile) != 0xff)
    return 0;
  if (jpeg_skip_1(ifile) != M_SOI)
    return 0;

  while (done == MagickFalse)
  {
    marker = jpeg_skip_till_marker(ifile, M_APP13);
    if (marker == M_APP13)
      {
        marker = jpeg_nextmarker(ifile, ofile);
        break;
      }
  }
  return 1;
}