void SplashOutputDev::updateFillOpacity(GfxState *state) {
  splash->setFillAlpha((SplashCoord)state->getFillOpacity());
  if (transpGroupStack != nullptr && (SplashCoord)state->getFillOpacity() < transpGroupStack->knockoutOpacity) {
    transpGroupStack->knockoutOpacity = (SplashCoord)state->getFillOpacity();
  }
}