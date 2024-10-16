void SplashOutputDev::updatePatternOpacity(GfxState *state) {
  splash->setPatternAlpha((SplashCoord)state->getStrokeOpacity(), (SplashCoord)state->getFillOpacity());
}