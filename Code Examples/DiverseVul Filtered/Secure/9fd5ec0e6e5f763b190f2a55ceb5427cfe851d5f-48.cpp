PDFDoc::PDFDoc(BaseStream *strA, const GooString *ownerPassword,
	       const GooString *userPassword, void *guiDataA) {
#ifdef _WIN32
  int n, i;
#endif

  init();
  guiData = guiDataA;
  if (strA->getFileName()) {
    fileName = strA->getFileName()->copy();
#ifdef _WIN32
    n = fileName->getLength();
    fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
    for (i = 0; i < n; ++i) {
      fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
    }
    fileNameU[n] = L'\0';
#endif
  } else {
    fileName = nullptr;
#ifdef _WIN32
    fileNameU = NULL;
#endif
  }
  str = strA;
  ok = setup(ownerPassword, userPassword);
}