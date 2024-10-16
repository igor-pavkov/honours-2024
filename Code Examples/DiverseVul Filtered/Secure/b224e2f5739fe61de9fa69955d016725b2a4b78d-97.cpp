bool SplashOutputDev::checkTransparencyGroup(GfxState *state, bool knockout) {
  if (state->getFillOpacity() != 1 || 
    state->getStrokeOpacity() != 1 ||
    state->getAlphaIsShape() ||
    state->getBlendMode() != gfxBlendNormal ||
    splash->getSoftMask() != nullptr ||
    knockout) 
    return true;
  return transpGroupStack != nullptr && transpGroupStack->shape != nullptr;
}