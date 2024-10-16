void SplashOutputDev::eoClip(GfxState *state) {
  SplashPath path = convertPath(state, state->getPath(), true);
  splash->clipToPath(&path, true);
}