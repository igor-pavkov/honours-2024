void SplashOutputDev::updateFlatness(GfxState *state) {
#if 0 // Acrobat ignores the flatness setting, and always renders curves
      // with a fairly small flatness value
   splash->setFlatness(state->getFlatness());
#endif
}