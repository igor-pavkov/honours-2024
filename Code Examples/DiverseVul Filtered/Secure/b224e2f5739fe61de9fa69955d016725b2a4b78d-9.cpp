void SplashOutputDev::beginTransparencyGroup(GfxState *state, const double *bbox,
					     GfxColorSpace *blendingColorSpace,
					     bool isolated, bool knockout,
					     bool forSoftMask) {
  SplashTransparencyGroup *transpGroup;
  SplashColor color;
  double xMin, yMin, xMax, yMax, x, y;
  int tx, ty, w, h, i;

  // transform the bbox
  state->transform(bbox[0], bbox[1], &x, &y);
  xMin = xMax = x;
  yMin = yMax = y;
  state->transform(bbox[0], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[1], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  tx = (int)floor(xMin);
  if (tx < 0) {
    tx = 0;
  } else if (tx >= bitmap->getWidth()) {
    tx = bitmap->getWidth() - 1;
  }
  ty = (int)floor(yMin);
  if (ty < 0) {
    ty = 0;
  } else if (ty >= bitmap->getHeight()) {
    ty = bitmap->getHeight() - 1;
  }
  w = (int)ceil(xMax) - tx + 1;
  if (tx + w > bitmap->getWidth()) {
    w = bitmap->getWidth() - tx;
  }
  if (w < 1) {
    w = 1;
  }
  h = (int)ceil(yMax) - ty + 1;
  if (ty + h > bitmap->getHeight()) {
    h = bitmap->getHeight() - ty;
  }
  if (h < 1) {
    h = 1;
  }

  // push a new stack entry
  transpGroup = new SplashTransparencyGroup();
  transpGroup->softmask = nullptr;
  transpGroup->tx = tx;
  transpGroup->ty = ty;
  transpGroup->blendingColorSpace = blendingColorSpace;
  transpGroup->isolated = isolated;
  transpGroup->shape = (knockout && !isolated) ? SplashBitmap::copy(bitmap) : nullptr;
  transpGroup->knockout = (knockout && isolated);
  transpGroup->knockoutOpacity = 1.0;
  transpGroup->next = transpGroupStack;
  transpGroupStack = transpGroup;

  // save state
  transpGroup->origBitmap = bitmap;
  transpGroup->origSplash = splash;
  transpGroup->fontAA = fontEngine->getAA();

  //~ this handles the blendingColorSpace arg for soft masks, but
  //~   not yet for transparency groups

  // switch to the blending color space
  if (forSoftMask && isolated && blendingColorSpace) {
    if (blendingColorSpace->getMode() == csDeviceGray ||
	blendingColorSpace->getMode() == csCalGray ||
	(blendingColorSpace->getMode() == csICCBased &&
	 blendingColorSpace->getNComps() == 1)) {
      colorMode = splashModeMono8;
    } else if (blendingColorSpace->getMode() == csDeviceRGB ||
	       blendingColorSpace->getMode() == csCalRGB ||
	       (blendingColorSpace->getMode() == csICCBased &&
		blendingColorSpace->getNComps() == 3)) {
      //~ does this need to use BGR8?
      colorMode = splashModeRGB8;
#ifdef SPLASH_CMYK
    } else if (blendingColorSpace->getMode() == csDeviceCMYK ||
	       (blendingColorSpace->getMode() == csICCBased &&
		blendingColorSpace->getNComps() == 4)) {
      colorMode = splashModeCMYK8;
#endif
    }
  }

  // create the temporary bitmap
  bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode, true,
			    bitmapTopDown, bitmap->getSeparationList());
  if (!bitmap->getDataPtr()) {
    delete bitmap;
    w = h = 1;
    bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode, true, bitmapTopDown);
  }
  splash = new Splash(bitmap, vectorAntialias,
		      transpGroup->origSplash->getScreen());
  if (transpGroup->next != nullptr && transpGroup->next->knockout) {
    fontEngine->setAA(false);
  }
  splash->setThinLineMode(transpGroup->origSplash->getThinLineMode());
  splash->setMinLineWidth(s_minLineWidth);
  //~ Acrobat apparently copies at least the fill and stroke colors, and
  //~ maybe other state(?) -- but not the clipping path (and not sure
  //~ what else)
  //~ [this is likely the same situation as in type3D1()]
  splash->setFillPattern(transpGroup->origSplash->getFillPattern()->copy());
  splash->setStrokePattern(
		         transpGroup->origSplash->getStrokePattern()->copy());
  if (isolated) {
    for (i = 0; i < splashMaxColorComps; ++i) {
      color[i] = 0;
    }
    if (colorMode == splashModeXBGR8) color[3] = 255;
    splash->clear(color, 0);
  } else {
    SplashBitmap *shape = (knockout) ? transpGroup->shape :
                                       (transpGroup->next != nullptr && transpGroup->next->shape != nullptr) ? transpGroup->next->shape : transpGroup->origBitmap;
    int shapeTx = (knockout) ? tx :
      (transpGroup->next != nullptr && transpGroup->next->shape != nullptr) ? transpGroup->next->tx + tx : tx;
    int shapeTy = (knockout) ? ty :
      (transpGroup->next != nullptr && transpGroup->next->shape != nullptr) ? transpGroup->next->ty + ty : ty;
    splash->blitTransparent(transpGroup->origBitmap, tx, ty, 0, 0, w, h);
    splash->setInNonIsolatedGroup(shape, shapeTx, shapeTy);
  }
  transpGroup->tBitmap = bitmap;
  state->shiftCTMAndClip(-tx, -ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  ++nestCount;
}