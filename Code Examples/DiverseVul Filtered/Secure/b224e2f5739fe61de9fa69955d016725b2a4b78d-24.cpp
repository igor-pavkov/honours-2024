void SplashOutputDev::startDoc(PDFDoc *docA) {
  int i;

  doc = docA;
  if (fontEngine) {
    delete fontEngine;
  }
  fontEngine = new SplashFontEngine(
				    globalParams->getEnableFreeType(),
				    enableFreeTypeHinting,
				    enableSlightHinting,
				      getFontAntialias() &&
				      colorMode != splashModeMono1);
  for (i = 0; i < nT3Fonts; ++i) {
    delete t3FontCache[i];
  }
  nT3Fonts = 0;
}