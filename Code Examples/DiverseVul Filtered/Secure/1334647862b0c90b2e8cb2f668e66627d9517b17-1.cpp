void CLASS packed_load_raw()
{
  int vbits=0, bwide, rbits, bite, half, irow, row, col, val, i;
  UINT64 bitbuf=0;

  bwide = raw_width * tiff_bps / 8;
  bwide += bwide & load_flags >> 7;
  rbits = bwide * 8 - raw_width * tiff_bps;
  if (load_flags & 1) bwide = bwide * 16 / 15;
  bite = 8 + (load_flags & 24);
  half = (raw_height+1) >> 1;
  for (irow=0; irow < raw_height; irow++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    row = irow;
    if (load_flags & 2 &&
	(row = irow % half * 2 + irow / half) == 1 &&
	load_flags & 4) {
      if (vbits=0, tiff_compress)
	fseek (ifp, data_offset - (-half*bwide & -2048), SEEK_SET);
      else {
	fseek (ifp, 0, SEEK_END);
	fseek (ifp, ftell(ifp) >> 3 << 2, SEEK_SET);
      }
    }
    if(feof(ifp)) throw LIBRAW_EXCEPTION_IO_EOF;
    for (col=0; col < raw_width; col++) {
      for (vbits -= tiff_bps; vbits < 0; vbits += bite) {
	bitbuf <<= bite;
	for (i=0; i < bite; i+=8)
	  bitbuf |= (unsigned) (fgetc(ifp) << i);
      }
      val = bitbuf << (64-tiff_bps-vbits) >> (64-tiff_bps);
      RAW(row,col ^ (load_flags >> 6 & 1)) = val;
      if (load_flags & 1 && (col % 10) == 9 && fgetc(ifp) &&
	row < height+top_margin && col < width+left_margin) derror();
    }
    vbits -= rbits;
  }
}