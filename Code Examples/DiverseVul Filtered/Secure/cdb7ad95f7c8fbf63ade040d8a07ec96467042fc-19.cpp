void FoFiTrueType::convertToType0(char *psName, int *cidMap, int nCIDs,
				  GBool needVerticalMetrics,
				  FoFiOutputFunc outputFunc,
				  void *outputStream) {
  GooString *buf;
  GooString *sfntsName;
  int maxUsedGlyph, n, i, j;

  if (openTypeCFF) {
    return;
  }

  // write the Type 42 sfnts array
  sfntsName = (new GooString(psName))->append("_sfnts");
  cvtSfnts(outputFunc, outputStream, sfntsName, needVerticalMetrics,
	   &maxUsedGlyph);
  delete sfntsName;

  // write the descendant Type 42 fonts
  // (The following is a kludge: nGlyphs is the glyph count from the
  // maxp table; maxUsedGlyph is the max glyph number that has a
  // non-zero-length description, from the loca table.  The problem is
  // that some TrueType font subsets fail to change the glyph count,
  // i.e., nGlyphs is much larger than maxUsedGlyph+1, which results
  // in an unnecessarily huge Type 0 font.  But some other PDF files
  // have fonts with only zero or one used glyph, and a content stream
  // that refers to one of the unused glyphs -- this results in PS
  // errors if we simply use maxUsedGlyph+1 for the Type 0 font.  So
  // we compromise by always defining at least 256 glyphs.)
  if (cidMap) {
    n = nCIDs;
  } else if (nGlyphs > maxUsedGlyph + 256) {
    if (maxUsedGlyph <= 255) {
      n = 256;
    } else {
      n = maxUsedGlyph + 1;
    }
  } else {
    n = nGlyphs;
  }
  for (i = 0; i < n; i += 256) {
    (*outputFunc)(outputStream, "10 dict begin\n", 14);
    (*outputFunc)(outputStream, "/FontName /", 11);
    (*outputFunc)(outputStream, psName, strlen(psName));
    buf = GooString::format("_{0:02x} def\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/FontType 42 def\n", 17);
    (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
    buf = GooString::format("/FontBBox [{0:d} {1:d} {2:d} {3:d}] def\n",
			  bbox[0], bbox[1], bbox[2], bbox[3]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/PaintType 0 def\n", 17);
    (*outputFunc)(outputStream, "/sfnts ", 7);
    (*outputFunc)(outputStream, psName, strlen(psName));
    (*outputFunc)(outputStream, "_sfnts def\n", 11);
    (*outputFunc)(outputStream, "/Encoding 256 array\n", 20);
    for (j = 0; j < 256 && i+j < n; ++j) {
      buf = GooString::format("dup {0:d} /c{1:02x} put\n", j, j);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "readonly def\n", 13);
    (*outputFunc)(outputStream, "/CharStrings 257 dict dup begin\n", 32);
    (*outputFunc)(outputStream, "/.notdef 0 def\n", 15);
    for (j = 0; j < 256 && i+j < n; ++j) {
      buf = GooString::format("/c{0:02x} {1:d} def\n",
			    j, cidMap ? cidMap[i+j] : i+j);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "end readonly def\n", 17);
    (*outputFunc)(outputStream,
		  "FontName currentdict end definefont pop\n", 40);
  }

  // write the Type 0 parent font
  (*outputFunc)(outputStream, "16 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/FontType 0 def\n", 16);
  (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  (*outputFunc)(outputStream, "/FMapType 2 def\n", 16);
  (*outputFunc)(outputStream, "/Encoding [\n", 12);
  for (i = 0; i < n; i += 256) {
    buf = GooString::format("{0:d}\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "/FDepVector [\n", 14);
  for (i = 0; i < n; i += 256) {
    (*outputFunc)(outputStream, "/", 1);
    (*outputFunc)(outputStream, psName, strlen(psName));
    buf = GooString::format("_{0:02x} findfont\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "FontName currentdict end definefont pop\n", 40);
}