void SplashOutputDev::restoreState(GfxState *state) {
  splash->restoreState();
  needFontUpdate = true;
  if (t3GlyphStack && !t3GlyphStack->haveDx) {
    t3GlyphStack->doNotCache = true;
    error(errSyntaxWarning, -1,
	  "Restore (Q) operator before d0/d1 in Type 3 glyph");
  }
}