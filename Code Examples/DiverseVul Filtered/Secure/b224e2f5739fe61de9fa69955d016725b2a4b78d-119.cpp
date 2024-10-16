SplashOutputDev::SplashOutputDev(SplashColorMode colorModeA,
				 int bitmapRowPadA,
				 bool reverseVideoA,
				 SplashColorPtr paperColorA,
				 bool bitmapTopDownA,
				 SplashThinLineMode thinLineMode,
				 bool overprintPreviewA) {
  colorMode = colorModeA;
  bitmapRowPad = bitmapRowPadA;
  bitmapTopDown = bitmapTopDownA;
  bitmapUpsideDown = false;
  fontAntialias = true;
  vectorAntialias = true;
  overprintPreview = overprintPreviewA;
  enableFreeTypeHinting = false;
  enableSlightHinting = false;
  setupScreenParams(72.0, 72.0);
  reverseVideo = reverseVideoA;
  if (paperColorA != nullptr) {
    splashColorCopy(paperColor, paperColorA);
  } else {
    splashClearColor(paperColor);
  }
  skipHorizText = false;
  skipRotatedText = false;
  keepAlphaChannel = paperColorA == nullptr;

  doc = nullptr;

  bitmap = new SplashBitmap(1, 1, bitmapRowPad, colorMode,
			    colorMode != splashModeMono1, bitmapTopDown);
  splash = new Splash(bitmap, vectorAntialias, &screenParams);
  splash->setMinLineWidth(s_minLineWidth);
  splash->setThinLineMode(thinLineMode);
  splash->clear(paperColor, 0);

  fontEngine = nullptr;

  nT3Fonts = 0;
  t3GlyphStack = nullptr;

  font = nullptr;
  needFontUpdate = false;
  textClipPath = nullptr;
  transpGroupStack = nullptr;
  nestCount = 0;
  xref = nullptr;
}