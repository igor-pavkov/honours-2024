bool SplashOutputDev::functionShadedFill(GfxState *state, GfxFunctionShading *shading) {
  SplashFunctionPattern *pattern = new SplashFunctionPattern(colorMode, state, shading);
  double xMin, yMin, xMax, yMax;
  bool vaa = getVectorAntialias();
  // restore vector antialias because we support it here
  setVectorAntialias(true);

  bool retVal = false;
  // get the clip region bbox
  if (pattern->getShading()->getHasBBox()) {
    pattern->getShading()->getBBox(&xMin, &yMin, &xMax, &yMax);
  } else {
    state->getClipBBox(&xMin, &yMin, &xMax, &yMax);

    xMin = floor (xMin);
    yMin = floor (yMin);
    xMax = ceil (xMax);
    yMax = ceil (yMax);

    {
      Matrix ctm, ictm;
      double x[4], y[4];
      int i;

      state->getCTM(&ctm);
      ctm.invertTo(&ictm);

      ictm.transform(xMin, yMin, &x[0], &y[0]);
      ictm.transform(xMax, yMin, &x[1], &y[1]);
      ictm.transform(xMin, yMax, &x[2], &y[2]);
      ictm.transform(xMax, yMax, &x[3], &y[3]);

      xMin = xMax = x[0];
      yMin = yMax = y[0];
      for (i = 1; i < 4; i++) {
        xMin = std::min<double>(xMin, x[i]);
        yMin = std::min<double>(yMin, y[i]);
        xMax = std::max<double>(xMax, x[i]);
        yMax = std::max<double>(yMax, y[i]);
      }
    }
  }

  // fill the region
  state->moveTo(xMin, yMin);
  state->lineTo(xMax, yMin);
  state->lineTo(xMax, yMax);
  state->lineTo(xMin, yMax);
  state->closePath();
  SplashPath path = convertPath(state, state->getPath(), true);

#ifdef SPLASH_CMYK
  pattern->getShading()->getColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
#endif
  setOverprintMask(pattern->getShading()->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), nullptr);
  retVal = (splash->shadedFill(&path, pattern->getShading()->getHasBBox(), pattern) == splashOk);
  state->clearPath();
  setVectorAntialias(vaa);

  delete pattern;

  return retVal;
}