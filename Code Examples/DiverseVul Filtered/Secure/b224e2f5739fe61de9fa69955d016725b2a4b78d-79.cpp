void SplashOutputDev::updateFillColorSpace(GfxState *state) {
#ifdef SPLASH_CMYK
  if (colorMode == splashModeDeviceN8)
    state->getFillColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
#endif
}