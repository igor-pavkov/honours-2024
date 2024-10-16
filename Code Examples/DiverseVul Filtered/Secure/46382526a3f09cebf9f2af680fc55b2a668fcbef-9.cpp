static int jpeg_read_remaining(Image *ifile, Image *ofile)
{
   int c;

  while ((c = jpeg_transfer_1(ifile, ofile)) != EOF)
    continue;
  return M_EOI;
}