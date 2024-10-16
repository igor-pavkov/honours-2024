static int jpeg_skip_variable2(Image *ifile, Image *ofile)
{
  unsigned int  length;
  int c1,c2;

  (void) ofile;
  if ((c1 = ReadBlobByte(ifile)) == EOF) return M_EOI;
  if ((c2 = ReadBlobByte(ifile)) == EOF) return M_EOI;

  length = (((unsigned char) c1) << 8) + ((unsigned char) c2);
  length -= 2;

  while (length--)
    if (ReadBlobByte(ifile) == EOF)
      return M_EOI;

  return 0;
}