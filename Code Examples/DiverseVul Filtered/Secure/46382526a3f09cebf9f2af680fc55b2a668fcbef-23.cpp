static int jpeg_embed(Image *ifile, Image *ofile, Image *iptc)
{
  unsigned int marker;
  unsigned int done = 0;
  unsigned int len;
  int inx;

  if (jpeg_transfer_1(ifile, ofile) != 0xFF)
    return 0;
  if (jpeg_transfer_1(ifile, ofile) != M_SOI)
    return 0;

  while (done == MagickFalse)
  {
    marker=(unsigned int) jpeg_nextmarker(ifile, ofile);
    if (marker == M_EOI)
      { /* EOF */
        break;
      }
    else
      {
        if (marker != M_APP13)
          {
            (void) WriteBlobByte(ofile,0xff);
            (void) WriteBlobByte(ofile,(unsigned char) marker);
          }
      }

    switch (marker)
    {
      case M_APP13:
        /* we are going to write a new APP13 marker, so don't output the old one */
        jpeg_skip_variable2(ifile, ofile);
        break;

      case M_APP0:
        /* APP0 is in each and every JPEG, so when we hit APP0 we insert our new APP13! */
        jpeg_skip_variable(ifile, ofile);

        if (iptc != (Image *) NULL)
          {
            char
              psheader[] = "\xFF\xED\0\0Photoshop 3.0\0" "8BIM\x04\x04\0\0\0\0";

            len=(unsigned int) GetBlobSize(iptc);
            if (len & 1)
              len++; /* make the length even */
            psheader[2]=(char) ((len+16)>>8);
            psheader[3]=(char) ((len+16)&0xff);
            for (inx = 0; inx < 18; inx++)
              (void) WriteBlobByte(ofile,(unsigned char) psheader[inx]);
            jpeg_read_remaining(iptc, ofile);
            len=(unsigned int) GetBlobSize(iptc);
            if (len & 1)
              (void) WriteBlobByte(ofile,0);
          }
        break;

      case M_SOS:
        /* we hit data, no more marker-inserting can be done! */
        jpeg_read_remaining(ifile, ofile);
        done = 1;
        break;

      default:
        jpeg_skip_variable(ifile, ofile);
        break;
    }
  }
  return 1;
}