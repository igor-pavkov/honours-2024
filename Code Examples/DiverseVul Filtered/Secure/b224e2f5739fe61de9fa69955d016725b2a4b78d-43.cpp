void SplashOutputDev::saveState(GfxState *state) {
  splash->saveState();
  if (t3GlyphStack && !t3GlyphStack->haveDx) {
    t3GlyphStack->doNotCache = true;
    error(errSyntaxWarning, -1,
	  "Save (q) operator before d0/d1 in Type 3 glyph");
  }
}