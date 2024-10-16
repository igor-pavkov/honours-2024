void SplashOutputDev::paintTransparencyGroup(GfxState *state, const double *bbox) {
  SplashBitmap *tBitmap;
  SplashTransparencyGroup *transpGroup;
  bool isolated;
  int tx, ty;

  tx = transpGroupStack->tx;
  ty = transpGroupStack->ty;
  tBitmap = transpGroupStack->tBitmap;
  isolated = transpGroupStack->isolated;

  // paint the transparency group onto the parent bitmap
  // - the clip path was set in the parent's state)
  if (tx < bitmap->getWidth() && ty < bitmap->getHeight()) {
    SplashCoord knockoutOpacity = (transpGroupStack->next != nullptr) ? transpGroupStack->next->knockoutOpacity
                                                                   : transpGroupStack->knockoutOpacity;
    splash->setOverprintMask(0xffffffff, false);
    splash->composite(tBitmap, 0, 0, tx, ty,
      tBitmap->getWidth(), tBitmap->getHeight(),
      false, !isolated, transpGroupStack->next != nullptr && transpGroupStack->next->knockout, knockoutOpacity);
    fontEngine->setAA(transpGroupStack->fontAA);
    if (transpGroupStack->next != nullptr && transpGroupStack->next->shape != nullptr) {
      transpGroupStack->next->knockout = true;
    }
  }

  // pop the stack
  transpGroup = transpGroupStack;
  transpGroupStack = transpGroup->next;
  if (transpGroupStack != nullptr && transpGroup->knockoutOpacity < transpGroupStack->knockoutOpacity) {
    transpGroupStack->knockoutOpacity = transpGroup->knockoutOpacity;
  }
  delete transpGroup->shape;
  delete transpGroup;

  delete tBitmap;
}