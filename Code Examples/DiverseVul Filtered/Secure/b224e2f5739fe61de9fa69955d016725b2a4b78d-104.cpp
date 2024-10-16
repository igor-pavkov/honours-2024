void SplashOutputDev::doUpdateFont(GfxState *state) {
  GfxFont *gfxFont;
  GfxFontLoc *fontLoc;
  GfxFontType fontType;
  SplashOutFontFileID *id = nullptr;
  SplashFontFile *fontFile;
  SplashFontSrc *fontsrc = nullptr;
  FoFiTrueType *ff;
  GooString *fileName;
  char *tmpBuf;
  int tmpBufLen;
  int *codeToGID;
  const double *textMat;
  double m11, m12, m21, m22, fontSize;
  int faceIndex = 0;
  SplashCoord mat[4];
  int n, i;
  bool recreateFont = false;
  bool doAdjustFontMatrix = false;

  needFontUpdate = false;
  font = nullptr;
  fileName = nullptr;
  tmpBuf = nullptr;
  fontLoc = nullptr;

  if (!(gfxFont = state->getFont())) {
    goto err1;
  }
  fontType = gfxFont->getType();
  if (fontType == fontType3) {
    goto err1;
  }

  // sanity-check the font size - skip anything larger than 10 inches
  // (this avoids problems allocating memory for the font cache)
  if (state->getTransformedFontSize()
        > 10 * (state->getHDPI() + state->getVDPI())) {
    goto err1;
  }

  // check the font file cache
reload:
  delete id;
  delete fontLoc;
  fontLoc = nullptr;
  if (fontsrc && !fontsrc->isFile) {
      fontsrc->unref();
      fontsrc = nullptr;
  }

  id = new SplashOutFontFileID(gfxFont->getID());
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  } else {

    if (!(fontLoc = gfxFont->locateFont((xref) ? xref : doc->getXRef(), nullptr))) {
      error(errSyntaxError, -1, "Couldn't find a font for '{0:s}'",
	    gfxFont->getName() ? gfxFont->getName()->c_str()
	                       : "(unnamed)");
      goto err2;
    }

    // embedded font
    if (fontLoc->locType == gfxFontLocEmbedded) {
      // if there is an embedded font, read it to memory
      tmpBuf = gfxFont->readEmbFontFile((xref) ? xref : doc->getXRef(), &tmpBufLen);
      if (! tmpBuf)
	goto err2;

    // external font
    } else { // gfxFontLocExternal
      fileName = fontLoc->path;
      fontType = fontLoc->fontType;
      doAdjustFontMatrix = true;
    }

    fontsrc = new SplashFontSrc;
    if (fileName)
      fontsrc->setFile(fileName, false);
    else
      fontsrc->setBuf(tmpBuf, tmpBufLen, true);

    // load the font file
    switch (fontType) {
    case fontType1:
      if (!(fontFile = fontEngine->loadType1Font(
			   id,
			   fontsrc,
			   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontType1C:
      if (!(fontFile = fontEngine->loadType1CFont(
			   id,
			   fontsrc,
			   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontType1COT:
      if (!(fontFile = fontEngine->loadOpenTypeT1CFont(
			   id,
			   fontsrc,
			   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontTrueType:
    case fontTrueTypeOT:
	if (fileName)
	 ff = FoFiTrueType::load(fileName->c_str());
	else
	ff = FoFiTrueType::make(tmpBuf, tmpBufLen);
      if (ff) {
	codeToGID = ((Gfx8BitFont *)gfxFont)->getCodeToGIDMap(ff);
	n = 256;
	delete ff;
	// if we're substituting for a non-TrueType font, we need to mark
	// all notdef codes as "do not draw" (rather than drawing TrueType
	// notdef glyphs)
	if (gfxFont->getType() != fontTrueType &&
	    gfxFont->getType() != fontTrueTypeOT) {
	  for (i = 0; i < 256; ++i) {
	    if (codeToGID[i] == 0) {
	      codeToGID[i] = -1;
	    }
	  }
	}
      } else {
	codeToGID = nullptr;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
			   fontsrc,
			   codeToGID, n))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontCIDType0:
    case fontCIDType0C:
      if (!(fontFile = fontEngine->loadCIDFont(
			   id,
			   fontsrc))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontCIDType0COT:
      if (((GfxCIDFont *)gfxFont)->getCIDToGID()) {
	n = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
	codeToGID = (int *)gmallocn(n, sizeof(int));
	memcpy(codeToGID, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
	       n * sizeof(int));
      } else {
	codeToGID = nullptr;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadOpenTypeCFFFont(
			   id,
			   fontsrc,
                           codeToGID, n))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    case fontCIDType2:
    case fontCIDType2OT:
      codeToGID = nullptr;
      n = 0;
      if (((GfxCIDFont *)gfxFont)->getCIDToGID()) {
	n = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
	if (n) {
	  codeToGID = (int *)gmallocn(n, sizeof(int));
	  memcpy(codeToGID, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
		  n * sizeof(int));
	}
      } else {
	if (fileName)
	  ff = FoFiTrueType::load(fileName->c_str());
	else
	  ff = FoFiTrueType::make(tmpBuf, tmpBufLen);
	if (! ff)
	{
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	  goto err2;
	}
	codeToGID = ((GfxCIDFont *)gfxFont)->getCodeToGIDMap(ff, &n);
	delete ff;
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
			   fontsrc,
			   codeToGID, n, faceIndex))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->c_str()
	                         : "(unnamed)");
	if (gfxFont->invalidateEmbeddedFont()) goto reload;
	goto err2;
      }
      break;
    default:
      // this shouldn't happen
      goto err2;
    }
    fontFile->doAdjustMatrix = doAdjustFontMatrix;
  }

  // get the font matrix
  textMat = state->getTextMat();
  fontSize = state->getFontSize();
  m11 = textMat[0] * fontSize * state->getHorizScaling();
  m12 = textMat[1] * fontSize * state->getHorizScaling();
  m21 = textMat[2] * fontSize;
  m22 = textMat[3] * fontSize;

  // create the scaled font
  mat[0] = m11;  mat[1] = m12;
  mat[2] = m21;  mat[3] = m22;
  font = fontEngine->getFont(fontFile, mat, splash->getMatrix());

  // for substituted fonts: adjust the font matrix -- compare the
  // width of 'm' in the original font and the substituted font
  if (fontFile->doAdjustMatrix && !gfxFont->isCIDFont()) {
    double w1, w2, w3;
    CharCode code;
    const char *name;
    for (code = 0; code < 256; ++code) {
      if ((name = ((Gfx8BitFont *)gfxFont)->getCharName(code)) &&
          name[0] == 'm' && name[1] == '\0') {
        break;
      }
    }
    if (code < 256) {
      w1 = ((Gfx8BitFont *)gfxFont)->getWidth(code);
      w2 = font->getGlyphAdvance(code);
      w3 = ((Gfx8BitFont *)gfxFont)->getWidth(0);
      if (!gfxFont->isSymbolic() && w2 > 0 && w1 > w3) {
        // if real font is substantially narrower than substituted
        // font, reduce the font size accordingly
        if (w1 > 0.01 && w1 < 0.9 * w2) {
          w1 /= w2;
          m11 *= w1;
          m21 *= w1;
          recreateFont = true;
        }
      }
    }
  }

  if (recreateFont)
  {
    mat[0] = m11;  mat[1] = m12;
    mat[2] = m21;  mat[3] = m22;
    font = fontEngine->getFont(fontFile, mat, splash->getMatrix());
  }

  delete fontLoc;
  if (fontsrc && !fontsrc->isFile)
      fontsrc->unref();
  return;

 err2:
  delete id;
  delete fontLoc;
 err1:
  if (fontsrc && !fontsrc->isFile)
      fontsrc->unref();
  return;
}