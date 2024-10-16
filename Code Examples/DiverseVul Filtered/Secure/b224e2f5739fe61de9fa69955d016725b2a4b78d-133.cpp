void SplashOutputDev::drawType3Glyph(GfxState *state, T3FontCache *t3Font,
				     T3FontCacheTag * /*tag*/, unsigned char *data) {
  SplashGlyphBitmap glyph;

  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  glyph.x = -t3Font->glyphX;
  glyph.y = -t3Font->glyphY;
  glyph.w = t3Font->glyphW;
  glyph.h = t3Font->glyphH;
  glyph.aa = colorMode != splashModeMono1;
  glyph.data = data;
  glyph.freeData = false;
  splash->fillGlyph(0, 0, &glyph);
}