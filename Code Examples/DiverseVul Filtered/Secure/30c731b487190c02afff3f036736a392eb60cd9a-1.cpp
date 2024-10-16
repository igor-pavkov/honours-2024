    HtmlImage(GooString *_fName, GfxState *state)
      : fName(_fName) {
    state->transform(0, 0, &xMin, &yMax);
    state->transform(1, 1, &xMax, &yMin);
  }