void SplashOutputDev::endTransparencyGroup(GfxState *state) {
  // restore state
  --nestCount;
  delete splash;
  bitmap = transpGroupStack->origBitmap;
  colorMode = bitmap->getMode();
  splash = transpGroupStack->origSplash;
  state->shiftCTMAndClip(transpGroupStack->tx, transpGroupStack->ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
}