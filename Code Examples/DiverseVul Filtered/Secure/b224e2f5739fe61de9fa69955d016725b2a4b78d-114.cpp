void SplashOutputDev::drawChar(GfxState *state, double x, double y,
			       double dx, double dy,
			       double originX, double originY,
			       CharCode code, int nBytes,
			       Unicode *u, int uLen) {
  SplashPath *path;
  int render;
  bool doFill, doStroke, doClip, strokeAdjust;
  double m[4];
  bool horiz;

  if (skipHorizText || skipRotatedText) {
    state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
    horiz = m[0] > 0 && fabs(m[1]) < 0.001 &&
            fabs(m[2]) < 0.001 && m[3] < 0;
    if ((skipHorizText && horiz) || (skipRotatedText && !horiz)) {
      return;
    }
  }

  // check for invisible text -- this is used by Acrobat Capture
  render = state->getRender();
  if (render == 3) {
    return;
  }

  if (needFontUpdate) {
    doUpdateFont(state);
  }
  if (!font) {
    return;
  }

  x -= originX;
  y -= originY;

  doFill = !(render & 1) && !state->getFillColorSpace()->isNonMarking();
  doStroke = ((render & 3) == 1 || (render & 3) == 2) &&
             !state->getStrokeColorSpace()->isNonMarking();
  doClip = render & 4;

  path = nullptr;
  SplashCoord lineWidth = splash->getLineWidth();
  if (doStroke && lineWidth == 0.0)
    splash->setLineWidth(1 / state->getVDPI());
  if (doStroke || doClip) {
    if ((path = font->getGlyphPath(code))) {
      path->offset((SplashCoord)x, (SplashCoord)y);
    }
  }

  // don't use stroke adjustment when stroking text -- the results
  // tend to be ugly (because characters with horizontal upper or
  // lower edges get misaligned relative to the other characters)
  strokeAdjust = false; // make gcc happy
  if (doStroke) {
    strokeAdjust = splash->getStrokeAdjust();
    splash->setStrokeAdjust(false);
  }

  // fill and stroke
  if (doFill && doStroke) {
    if (path) {
      setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		       state->getOverprintMode(), state->getFillColor());
      splash->fill(path, false);
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      splash->stroke(path);
    }

  // fill
  } else if (doFill) {
    setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		     state->getOverprintMode(), state->getFillColor());
    splash->fillChar((SplashCoord)x, (SplashCoord)y, code, font);

  // stroke
  } else if (doStroke) {
    if (path) {
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      splash->stroke(path);
    }
  }
  splash->setLineWidth(lineWidth);

  // clip
  if (doClip) {
    if (path) {
      if (textClipPath) {
	textClipPath->append(path);
      } else {
	textClipPath = path;
	path = nullptr;
      }
    }
  }

  if (doStroke) {
    splash->setStrokeAdjust(strokeAdjust);
  }

  if (path) {
    delete path;
  }
}