static int jpeg_transfer_1(Image *ifile, Image *ofile)
{
  int c;

  c = ReadBlobByte(ifile);
  if (c == EOF)
    return EOF;
  (void) WriteBlobByte(ofile,(unsigned char) c);
  return c;
}