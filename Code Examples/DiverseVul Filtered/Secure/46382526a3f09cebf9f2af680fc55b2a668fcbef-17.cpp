static void jpeg_strip(Image *ifile, Image *ofile)
{
  unsigned int marker;

  marker = jpeg_skip_till_marker(ifile, M_SOI);
  if (marker == M_SOI)
  {
    (void) WriteBlobByte(ofile,0xff);
    (void) WriteBlobByte(ofile,M_SOI);
    jpeg_read_remaining(ifile, ofile);
  }
}