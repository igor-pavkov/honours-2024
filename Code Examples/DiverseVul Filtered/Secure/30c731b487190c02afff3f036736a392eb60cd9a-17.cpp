HtmlString::HtmlString(GfxState *state, double fontSize, HtmlFontAccu* _fonts) : fonts(_fonts) {
  GfxFont *font;
  double x, y;

  state->transform(state->getCurX(), state->getCurY(), &x, &y);
  if ((font = state->getFont())) {
    double ascent = font->getAscent();
    double descent = font->getDescent();
    if( ascent > 1.05 ){
        //printf( "ascent=%.15g is too high, descent=%.15g\n", ascent, descent );
        ascent = 1.05;
    }
    if( descent < -0.4 ){
        //printf( "descent %.15g is too low, ascent=%.15g\n", descent, ascent );
        descent = -0.4;
    }
    yMin = y - ascent * fontSize;
    yMax = y - descent * fontSize;
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    HtmlFont hfont=HtmlFont(font, static_cast<int>(fontSize-1), rgb);
    if (isMatRotOrSkew(state->getTextMat())) {
      double normalizedMatrix[4];
      memcpy(normalizedMatrix, state->getTextMat(), sizeof(normalizedMatrix));
      // browser rotates the opposite way
      // so flip the sign of the angle -> sin() components change sign
      if (debug)
        std::cerr << DEBUG << "before transform: " << print_matrix(normalizedMatrix) << std::endl;
      normalizedMatrix[1] *= -1;
      normalizedMatrix[2] *= -1;
      if (debug)
        std::cerr << DEBUG << "after reflecting angle: " << print_matrix(normalizedMatrix) << std::endl;
      normalizeRotMat(normalizedMatrix);
      if (debug)
        std::cerr << DEBUG << "after norm: " << print_matrix(normalizedMatrix) << std::endl;
      hfont.setRotMat(normalizedMatrix);
    }
    fontpos = fonts->AddFont(hfont);
  } else {
    // this means that the PDF file draws text without a current font,
    // which should never happen
    yMin = y - 0.95 * fontSize;
    yMax = y + 0.35 * fontSize;
    fontpos=0;
  }
  if (yMin == yMax) {
    // this is a sanity check for a case that shouldn't happen -- but
    // if it does happen, we want to avoid dividing by zero later
    yMin = y;
    yMax = y + 1;
  }
  col = 0;
  text = nullptr;
  xRight = nullptr;
  link = nullptr;
  len = size = 0;
  yxNext = nullptr;
  xyNext = nullptr;
  htext=new GooString();
  dir = textDirUnknown;
}