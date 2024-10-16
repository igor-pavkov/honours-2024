FontInfo::FontInfo(GfxFont *font, XRef *xref) {
  const GooString *origName;

  fontRef = *font->getID();

  // font name
  origName = font->getName();
  if (origName != nullptr) {
    name = font->getName()->copy();
  } else {
    name = nullptr;
  }

  // font type
  type = (FontInfo::Type)font->getType();

  // check for an embedded font
  if (font->getType() == fontType3) {
    emb = true;
  } else {
    emb = font->getEmbeddedFontID(&embRef);
  }

  file = nullptr;
  substituteName = nullptr;
  if (!emb)
  {
    SysFontType dummy;
    int dummy2;
    GooString substituteNameAux;
    file = globalParams->findSystemFontFile(font, &dummy, &dummy2, &substituteNameAux);
    if (substituteNameAux.getLength() > 0)
	substituteName = substituteNameAux.copy();
  }
  encoding = font->getEncodingName()->copy();

  // look for a ToUnicode map
  hasToUnicode = false;
  Object fontObj = xref->fetch(fontRef);
  if (fontObj.isDict()) {
    hasToUnicode = fontObj.dictLookup("ToUnicode").isStream();
  }

  // check for a font subset name: capital letters followed by a '+'
  // sign
  subset = false;
  if (name) {
    int i;
    for (i = 0; i < name->getLength(); ++i) {
      if (name->getChar(i) < 'A' || name->getChar(i) > 'Z') {
	break;
      }
    }
    subset = i > 0 && i < name->getLength() && name->getChar(i) == '+';
  }
}