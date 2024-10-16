void SplashOutputDev::type3D0(GfxState *state, double wx, double wy) {
  if (likely(t3GlyphStack != nullptr)) {
    t3GlyphStack->haveDx = true;
  } else {
    error(errSyntaxWarning, -1, "t3GlyphStack was null in SplashOutputDev::type3D0");
  }
}