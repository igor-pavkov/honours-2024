void HtmlString::addChar(GfxState *state, double x, double y,
			 double dx, double dy, Unicode u) {
  if (dir == textDirUnknown) {
    //dir = UnicodeMap::getDirection(u);
    dir = textDirLeftRight;
  } 

  if (len == size) {
    size += 16;
    text = (Unicode *)grealloc(text, size * sizeof(Unicode));
    xRight = (double *)grealloc(xRight, size * sizeof(double));
  }
  text[len] = u;
  if (len == 0) {
    xMin = x;
  }
  xMax = xRight[len] = x + dx;
//printf("added char: %f %f xright = %f\n", x, dx, x+dx);
  ++len;
}