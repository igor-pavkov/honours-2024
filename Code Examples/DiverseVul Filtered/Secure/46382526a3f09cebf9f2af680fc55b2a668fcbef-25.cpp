static int jpeg_nextmarker(Image *ifile, Image *ofile)
{
  int c;

  /* transfer anything until we hit 0xff */
  do
  {
    c = ReadBlobByte(ifile);
    if (c == EOF)
      return M_EOI; /* we hit EOF */
    else
      if (c != 0xff)
        (void) WriteBlobByte(ofile,(unsigned char) c);
  } while (c != 0xff);

  /* get marker byte, swallowing possible padding */
  do
  {
    c = ReadBlobByte(ifile);
    if (c == EOF)
      return M_EOI; /* we hit EOF */
  } while (c == 0xff);

  return c;
}