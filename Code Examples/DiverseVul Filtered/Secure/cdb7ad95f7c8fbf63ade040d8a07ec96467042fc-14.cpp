void FoFiTrueType::cvtEncoding(char **encoding,
			       FoFiOutputFunc outputFunc,
			       void *outputStream) {
  const char *name;
  GooString *buf;
  int i;

  (*outputFunc)(outputStream, "/Encoding 256 array\n", 20);
  if (encoding) {
    for (i = 0; i < 256; ++i) {
      if (!(name = encoding[i])) {
	name = ".notdef";
      }
      buf = GooString::format("dup {0:d} /", i);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
      (*outputFunc)(outputStream, name, strlen(name));
      (*outputFunc)(outputStream, " put\n", 5);
    }
  } else {
    for (i = 0; i < 256; ++i) {
      buf = GooString::format("dup {0:d} /c{1:02x} put\n", i, i);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
  }
  (*outputFunc)(outputStream, "readonly def\n", 13);
}