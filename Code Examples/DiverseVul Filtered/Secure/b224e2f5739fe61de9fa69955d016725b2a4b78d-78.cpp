void SplashOutputDev::updateOverprintMode(GfxState *state) {
  splash->setOverprintMode(state->getOverprintMode());
}