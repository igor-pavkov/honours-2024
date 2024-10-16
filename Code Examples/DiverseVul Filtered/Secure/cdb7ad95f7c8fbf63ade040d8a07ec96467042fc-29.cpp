void FoFiTrueType::cvtCharStrings(char **encoding,
				  int *codeToGID,
				  FoFiOutputFunc outputFunc,
				  void *outputStream) {
  char *name;
  GooString *buf;
  char buf2[16];
  int i, k;

  // always define '.notdef'
  (*outputFunc)(outputStream, "/CharStrings 256 dict dup begin\n", 32);
  (*outputFunc)(outputStream, "/.notdef 0 def\n", 15);

  // if there's no 'cmap' table, punt
  if (nCmaps == 0) {
    goto err;
  }

  // map char name to glyph index:
  // 1. use encoding to map name to char code
  // 2. use codeToGID to map char code to glyph index
  // N.B. We do this in reverse order because font subsets can have
  //      weird encodings that use the same character name twice, and
  //      the first definition is probably the one we want.
  k = 0; // make gcc happy
  for (i = 255; i >= 0; --i) {
    if (encoding) {
      name = encoding[i];
    } else {
      sprintf(buf2, "c%02x", i);
      name = buf2;
    }
    if (name && strcmp(name, ".notdef")) {
      k = codeToGID[i];
      // note: Distiller (maybe Adobe's PS interpreter in general)
      // doesn't like TrueType fonts that have CharStrings entries
      // which point to nonexistent glyphs, hence the (k < nGlyphs)
      // test
      if (k > 0 && k < nGlyphs) {
	(*outputFunc)(outputStream, "/", 1);
	(*outputFunc)(outputStream, name, strlen(name));
	buf = GooString::format(" {0:d} def\n", k);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
    }
  }

 err:
  (*outputFunc)(outputStream, "end readonly def\n", 17);
}