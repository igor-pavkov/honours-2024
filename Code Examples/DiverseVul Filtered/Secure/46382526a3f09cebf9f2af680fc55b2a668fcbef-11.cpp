static int jpeg_skip_variable(Image *ifile, Image *ofile)
{
  unsigned int  length;
  int c1,c2;

  if ((c1 = jpeg_transfer_1(ifile, ofile)) == EOF)
    return M_EOI;
  if ((c2 = jpeg_transfer_1(ifile, ofile)) == EOF)
    return M_EOI;

  length = (((unsigned char) c1) << 8) + ((unsigned char) c2);
  length -= 2;

  while (length--)
    if (jpeg_transfer_1(ifile, ofile) == EOF)
      return M_EOI;

  return 0;
}