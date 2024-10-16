void SplashOutputDev::updateStrokeOverprint(GfxState *state) {
  splash->setStrokeOverprint(state->getStrokeOverprint());
}