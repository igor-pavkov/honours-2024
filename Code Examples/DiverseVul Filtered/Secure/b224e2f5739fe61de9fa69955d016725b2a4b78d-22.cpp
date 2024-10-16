void SplashOutputDev::startPage(int pageNum, GfxState *state, XRef *xrefA) {
  int w, h;
  SplashCoord mat[6];
  SplashColor color;

  xref = xrefA;
  if (state) {
    setupScreenParams(state->getHDPI(), state->getVDPI());
    w = (int)(state->getPageWidth() + 0.5);
    if (w <= 0) {
      w = 1;
    }
    h = (int)(state->getPageHeight() + 0.5);
    if (h <= 0) {
      h = 1;
    }
  } else {
    w = h = 1;
  }
  SplashThinLineMode thinLineMode = splashThinLineDefault;
  if (splash) {
    thinLineMode = splash->getThinLineMode();
    delete splash;
    splash = nullptr;
  }
  if (!bitmap || w != bitmap->getWidth() || h != bitmap->getHeight()) {
    if (bitmap) {
      delete bitmap;
      bitmap = nullptr;
    }
    bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode,
			      colorMode != splashModeMono1, bitmapTopDown);
    if (!bitmap->getDataPtr()) {
      delete bitmap;
      w = h = 1;
      bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode,
                              colorMode != splashModeMono1, bitmapTopDown);
    }
  }
  splash = new Splash(bitmap, vectorAntialias, &screenParams);
  splash->setThinLineMode(thinLineMode);
  splash->setMinLineWidth(s_minLineWidth);
  if (state) {
    const double *ctm = state->getCTM();
    mat[0] = (SplashCoord)ctm[0];
    mat[1] = (SplashCoord)ctm[1];
    mat[2] = (SplashCoord)ctm[2];
    mat[3] = (SplashCoord)ctm[3];
    mat[4] = (SplashCoord)ctm[4];
    mat[5] = (SplashCoord)ctm[5];
    splash->setMatrix(mat);
  }
  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    color[0] = 0;
    break;
  case splashModeXBGR8:
    color[3] = 255;
    // fallthrough
  case splashModeRGB8:
  case splashModeBGR8:
    color[0] = color[1] = color[2] = 0;
    break;
#ifdef SPLASH_CMYK
  case splashModeCMYK8:
    color[0] = color[1] = color[2] = color[3] = 0;
    break;
  case splashModeDeviceN8:
    for (int i = 0; i < 4 + SPOT_NCOMPS; i++)
      color[i] = 0;
    break;
#endif
  }
  splash->setStrokePattern(new SplashSolidColor(color));
  splash->setFillPattern(new SplashSolidColor(color));
  splash->setLineCap(splashLineCapButt);
  splash->setLineJoin(splashLineJoinMiter);
  splash->setLineDash(nullptr, 0, 0);
  splash->setMiterLimit(10);
  splash->setFlatness(1);
  // the SA parameter supposedly defaults to false, but Acrobat
  // apparently hardwires it to true
  splash->setStrokeAdjust(true);
  splash->clear(paperColor, 0);
}