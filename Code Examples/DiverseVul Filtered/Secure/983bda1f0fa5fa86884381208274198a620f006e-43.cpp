void CLASS scale_colors()
{
  unsigned bottom, right, size, row, col, ur, uc, i, x, y, c, sum[8];
  int val, dark, sat;
  double dsum[8], dmin, dmax;
  float scale_mul[4], fr, fc;
  ushort *img=0, *pix;

  FORC4 cblack[c] += black;
  if (user_mul[0])
    memcpy (pre_mul, user_mul, sizeof pre_mul);
  if (use_auto_wb || (use_camera_wb && cam_mul[0] == -1)) {
    memset (dsum, 0, sizeof dsum);
    bottom = MIN (greybox[1]+greybox[3], height);
    right  = MIN (greybox[0]+greybox[2], width);
    for (row=greybox[1]; row < bottom; row += 8)
      for (col=greybox[0]; col < right; col += 8) {
	memset (sum, 0, sizeof sum);
	for (y=row; y < row+8 && y < bottom; y++)
	  for (x=col; x < col+8 && x < right; x++)
	    FORC4 {
	      if (filters) {
		c = FC(y,x);
		val = BAYER(y,x);
	      } else
		val = image[y*width+x][c];
	      if (val > (int)(maximum-25)) goto skip_block;
	      if ((val -= cblack[c]) < 0) val = 0;
	      sum[c] += val;
	      sum[c+4]++;
	      if (filters) break;
	    }
	FORC(8) dsum[c] += sum[c];
skip_block: ;
      }
    FORC4 if (dsum[c]) pre_mul[c] = dsum[c+4] / dsum[c];
  }
  if (use_camera_wb && cam_mul[0] != -1) {
    memset (sum, 0, sizeof sum);
    for (row=0; row < 8; row++)
      for (col=0; col < 8; col++) {
	c = FC(row,col);
	if ((val = white[row][col] - cblack[c]) > 0)
	  sum[c] += val;
	sum[c+4]++;
      }
    if (sum[0] && sum[1] && sum[2] && sum[3])
      FORC4 pre_mul[c] = (float) sum[c+4] / sum[c];
    else if (cam_mul[0] && cam_mul[2])
      memcpy (pre_mul, cam_mul, sizeof pre_mul);
    else
      dcraw_message (DCRAW_NO_CAMERA_WB,_("%s: Cannot use camera white balance.\n"), ifname_display);
  }
  if (pre_mul[3] == 0) pre_mul[3] = colors < 4 ? pre_mul[1] : 1;
  dark = black;
  sat = maximum;
  if (threshold) wavelet_denoise();
  maximum -= black;
  for (dmin=DBL_MAX, dmax=c=0; c < 4; c++) {
    if (dmin > pre_mul[c])
	dmin = pre_mul[c];
    if (dmax < pre_mul[c])
	dmax = pre_mul[c];
  }
  if (!highlight) dmax = dmin;
  FORC4 scale_mul[c] = (pre_mul[c] /= dmax) * 65535.0 / maximum;
  dcraw_message(DCRAW_VERBOSE,
      _("Scaling with darkness %d, saturation %d, and\nmultipliers"), dark, sat);
  FORC4 dcraw_message(DCRAW_VERBOSE, " %f", pre_mul[c]);
  dcraw_message(DCRAW_VERBOSE, "\n");
  size = iheight*iwidth;
  for (i=0; i < size*4; i++) {
    val = image[0][i];
    if (!val) continue;
    val -= cblack[i & 3];
    val *= scale_mul[i & 3];
    image[0][i] = CLIP(val);
  }
  if ((aber[0] != 1 || aber[2] != 1) && colors == 3) {
    dcraw_message (DCRAW_VERBOSE,_("Correcting chromatic aberration...\n"));
    for (c=0; c < 4; c+=2) {
      if (aber[c] == 1) continue;
      img = (ushort *) malloc (size * sizeof *img);
      merror (img, "scale_colors()");
      for (i=0; i < size; i++)
	img[i] = image[i][c];
      for (row=0; row < iheight; row++) {
	ur = fr = (row - iheight*0.5) * aber[c] + iheight*0.5;
	if (ur > (unsigned)(iheight-2)) continue;
	fr -= ur;
	for (col=0; col < iwidth; col++) {
	  uc = fc = (col - iwidth*0.5) * aber[c] + iwidth*0.5;
	  if (uc > (unsigned)(iwidth-2)) continue;
	  fc -= uc;
	  pix = img + ur*iwidth + uc;
	  image[row*iwidth+col][c] =
	    (pix[     0]*(1-fc) + pix[       1]*fc) * (1-fr) +
	    (pix[iwidth]*(1-fc) + pix[iwidth+1]*fc) * fr;
	}
      }
      free(img);
    }
  }
}