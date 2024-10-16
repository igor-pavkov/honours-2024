static int jpeg_skip_till_marker(Image *ifile, int marker)
{
  int c, i;

  do
  {
    /* skip anything until we hit 0xff */
    i = 0;
    do
    {
      c = ReadBlobByte(ifile);
      i++;
      if (c == EOF)
        return M_EOI; /* we hit EOF */
    } while (c != 0xff);

    /* get marker byte, swallowing possible padding */
    do
    {
      c = ReadBlobByte(ifile);
      if (c == EOF)
        return M_EOI; /* we hit EOF */
    } while (c == 0xff);
  } while (c != marker);
  return c;
}