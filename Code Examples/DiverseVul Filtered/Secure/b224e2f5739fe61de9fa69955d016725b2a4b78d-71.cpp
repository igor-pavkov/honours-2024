void SplashOutputDev::setSoftMaskFromImageMask(GfxState *state,
					       Object *ref, Stream *str,
					       int width, int height,
					       bool invert,
					       bool inlineImg, double *baseMatrix) {
  const double *ctm;
  SplashCoord mat[6];
  SplashOutImageMaskData imgMaskData;
  Splash *maskSplash;
  SplashColor maskColor;
  double bbox[4] = {0, 0, 1, 1}; // default;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }

  ctm = state->getCTM();
  for (int i = 0; i < 6; ++i) {
    if (!std::isfinite(ctm[i])) return;
  }
  
  beginTransparencyGroup(state, bbox, nullptr, false, false, false);
  baseMatrix[4] -= transpGroupStack->tx;
  baseMatrix[5] -= transpGroupStack->ty;

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];
  imgMaskData.imgStr = new ImageStream(str, width, 1, 1);
  imgMaskData.imgStr->reset();
  imgMaskData.invert = invert ? 0 : 1;
  imgMaskData.width = width;
  imgMaskData.height = height;
  imgMaskData.y = 0;

  transpGroupStack->softmask = new SplashBitmap(bitmap->getWidth(), bitmap->getHeight(), 1, splashModeMono8, false);
  maskSplash = new Splash(transpGroupStack->softmask, vectorAntialias);
  maskColor[0] = 0;
  maskSplash->clear(maskColor);
  maskColor[0] = 0xff;
  maskSplash->setFillPattern(new SplashSolidColor(maskColor));
  maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,  width, height, mat, t3GlyphStack != nullptr);
  delete maskSplash;
  delete imgMaskData.imgStr;
  str->close();
}