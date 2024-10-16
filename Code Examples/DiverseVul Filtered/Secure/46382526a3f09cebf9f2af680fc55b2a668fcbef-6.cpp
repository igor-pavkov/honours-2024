static int jpeg_skip_1(Image *ifile)
{
  int c;

  c = ReadBlobByte(ifile);
  if (c == EOF)
    return EOF;
  return c;
}