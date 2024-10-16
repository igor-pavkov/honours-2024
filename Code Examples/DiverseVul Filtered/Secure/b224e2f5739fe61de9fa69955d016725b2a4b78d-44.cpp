void SplashOutputDev::updateStrokeColorSpace(GfxState *state) {
#ifdef SPLASH_CMYK
  if (colorMode == splashModeDeviceN8)
    state->getStrokeColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
#endif
}