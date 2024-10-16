void SplashOutputDev::clip(GfxState *state) {
  SplashPath path = convertPath(state, state->getPath(), true);
  splash->clipToPath(&path, false);
}