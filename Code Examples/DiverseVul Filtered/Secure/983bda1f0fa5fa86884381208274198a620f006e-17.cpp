void CLASS kodak_262_load_raw()
{
  static const uchar kodak_tree[2][26] =
  { { 0,1,5,1,1,2,0,0,0,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9 },
    { 0,3,1,1,1,1,1,2,0,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9 } };
  ushort *huff[2];
  uchar *pixel;
  int *strip, ns, c, row, col, chess, pi=0, pi1, pi2, pred, val;

  FORC(2) huff[c] = make_decoder (kodak_tree[c]);
  ns = (raw_height+63) >> 5;
  pixel = (uchar *) malloc (raw_width*32 + ns*4);
  merror (pixel, "kodak_262_load_raw()");
  strip = (int *) (pixel + raw_width*32);
  order = 0x4d4d;
  FORC(ns) strip[c] = get4();
  for (row=0; row < raw_height; row++) {
    if ((row & 31) == 0) {
      fseek (ifp, strip[row >> 5], SEEK_SET);
      getbits(-1);
      pi = 0;
    }
    for (col=0; col < raw_width; col++) {
      chess = (row + col) & 1;
      pi1 = chess ? pi-2           : pi-raw_width-1;
      pi2 = chess ? pi-2*raw_width : pi-raw_width+1;
      if (col <= chess) pi1 = -1;
      if (pi1 < 0) pi1 = pi2;
      if (pi2 < 0) pi2 = pi1;
      if (pi1 < 0 && col > 1) pi1 = pi2 = pi-2;
      pred = (pi1 < 0) ? 0 : (pixel[pi1] + pixel[pi2]) >> 1;
      pixel[pi] = val = pred + ljpeg_diff (huff[chess]);
      if (val >> 8) derror();
      val = curve[pixel[pi++]];
      if ((unsigned) (col-left_margin) < width)
	BAYER(row,col-left_margin) = val;
      else black += val;
    }
  }
  free (pixel);
  FORC(2) free (huff[c]);
  if (raw_width > width)
    black /= (raw_width - width) * height;
}