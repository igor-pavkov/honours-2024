void CLASS subtract (const char *fname)
{
  FILE *fp;
  int dim[3]={0,0,0}, comment=0, number=0, error=0, nd=0, c, row, col;
  ushort *pixel;

  if (!(fp = fopen (fname, "rb"))) {
    perror (fname);  return;
  }
  if (fgetc(fp) != 'P' || fgetc(fp) != '5') error = 1;
  while (!error && nd < 3 && (c = fgetc(fp)) != EOF) {
    if (c == '#')  comment = 1;
    if (c == '\n') comment = 0;
    if (comment) continue;
    if (isdigit(c)) number = 1;
    if (number) {
      if (isdigit(c)) dim[nd] = dim[nd]*10 + c -'0';
      else if (isspace(c)) {
	number = 0;  nd++;
      } else error = 1;
    }
  }
  if (error || nd < 3) {
    dcraw_message (DCRAW_ERROR,_("%s is not a valid PGM file!\n"), fname);
    fclose (fp);  return;
  } else if (dim[0] != width || dim[1] != height || dim[2] != 65535) {
    dcraw_message (DCRAW_ERROR,_("%s has the wrong dimensions!\n"), fname);
    fclose (fp);  return;
  }
  pixel = (ushort *) calloc (width, sizeof *pixel);
  merror (pixel, "subtract()");
  for (row=0; row < height; row++) {
    fread (pixel, 2, width, fp);
    for (col=0; col < width; col++)
      BAYER(row,col) = MAX (BAYER(row,col) - ntohs(pixel[col]), 0);
  }
  free (pixel);
  fclose (fp);
  memset (cblack, 0, sizeof cblack);
  black = 0;
}