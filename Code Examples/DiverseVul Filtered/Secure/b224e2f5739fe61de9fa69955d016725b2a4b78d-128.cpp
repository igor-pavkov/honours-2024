void SplashOutputDev::updateStrokeOpacity(GfxState *state) {
  splash->setStrokeAlpha((SplashCoord)state->getStrokeOpacity());
  if (transpGroupStack != nullptr && (SplashCoord)state->getStrokeOpacity() < transpGroupStack->knockoutOpacity) {
    transpGroupStack->knockoutOpacity = (SplashCoord)state->getStrokeOpacity();
  }
}