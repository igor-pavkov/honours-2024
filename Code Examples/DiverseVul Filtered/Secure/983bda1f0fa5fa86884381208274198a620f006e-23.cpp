void CLASS lossless_jpeg_load_raw()
{
  int jwide, jrow, jcol, val, jidx, c, i, j, row=0, col=0;
  struct jhead jh;
  int min=INT_MAX;
  ushort *rp;

  if (!ljpeg_start (&jh, 0)) return;
  jwide = jh.wide * jh.clrs;

  for (jrow=0; jrow < jh.high; jrow++) {
    rp = ljpeg_row (jrow, &jh);
    if (load_flags & 1)
      row = jrow & 1 ? height-1-jrow/2 : jrow/2;
    for (jcol=0; jcol < jwide; jcol++) {
      val = *rp++;
      if (jh.bits <= 12)
	val = curve[val & 0xfff];
      if (cr2_slice[0]) {
	jidx = jrow*jwide + jcol;
	i = jidx / (cr2_slice[1]*jh.high);
	if ((j = i >= cr2_slice[0]))
		 i  = cr2_slice[0];
	jidx -= i * (cr2_slice[1]*jh.high);
	row = jidx / cr2_slice[1+j];
	col = jidx % cr2_slice[1+j] + i*cr2_slice[1];
      }
      if (raw_width == 3984 && (col -= 2) < 0)
	col += (row--,raw_width);
      if ((unsigned) (row-top_margin) < height) {
	c = FC(row-top_margin,col-left_margin);
	if ((unsigned) (col-left_margin) < width) {
	  BAYER(row-top_margin,col-left_margin) = val;
	  if (min > val) min = val;
	} else if (col > 1 && (unsigned) (col-left_margin+2) >
		    (unsigned) (width+3))
	  cblack[c] += (cblack[4+c]++,val);
      }
      if (++col >= raw_width)
	col = (row++,0);
    }
  }
  ljpeg_end (&jh);
  FORC4 if (cblack[4+c]) cblack[c] /= cblack[4+c];
  if (!strcasecmp(make,"KODAK"))
    black = min;
}