void FoFiTrueType::convertToType42(char *psName, char **encoding,
				   int *codeToGID,
				   FoFiOutputFunc outputFunc,
				   void *outputStream) {
  GooString *buf;
  int maxUsedGlyph;
  GBool ok;

  if (openTypeCFF) {
    return;
  }

  // write the header
  ok = gTrue;
  buf = GooString::format("%!PS-TrueTypeFont-{0:2g}\n",
			(double)getS32BE(0, &ok) / 65536.0);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;

  // begin the font dictionary
  (*outputFunc)(outputStream, "10 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/FontType 42 def\n", 17);
  (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  buf = GooString::format("/FontBBox [{0:d} {1:d} {2:d} {3:d}] def\n",
			bbox[0], bbox[1], bbox[2], bbox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/PaintType 0 def\n", 17);

  // write the guts of the dictionary
  cvtEncoding(encoding, outputFunc, outputStream);
  cvtCharStrings(encoding, codeToGID, outputFunc, outputStream);
  cvtSfnts(outputFunc, outputStream, NULL, gFalse, &maxUsedGlyph);

  // end the dictionary and define the font
  (*outputFunc)(outputStream, "FontName currentdict end definefont pop\n", 40);
}