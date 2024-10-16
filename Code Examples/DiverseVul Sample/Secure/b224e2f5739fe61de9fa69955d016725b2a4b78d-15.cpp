SplashGouraudPattern::SplashGouraudPattern(bool bDirectColorTranslationA,
                                           GfxState *stateA, GfxGouraudTriangleShading *shadingA) {
  state = stateA;
  shading = shadingA;
  bDirectColorTranslation = bDirectColorTranslationA;
  gfxMode = shadingA->getColorSpace()->getMode();
}