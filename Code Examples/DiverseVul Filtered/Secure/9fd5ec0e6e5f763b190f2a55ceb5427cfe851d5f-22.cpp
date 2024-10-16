PDFDoc::PDFDoc(const GooString *fileNameA, const GooString *ownerPassword,
	       const GooString *userPassword, void *guiDataA) {
#ifdef _WIN32
  int n, i;
#endif

  init();

  fileName = fileNameA;
  guiData = guiDataA;
#ifdef _WIN32
  n = fileName->getLength();
  fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
  for (i = 0; i < n; ++i) {
    fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
  }
  fileNameU[n] = L'\0';
#endif

  // try to open file
#ifdef _WIN32
  wchar_t *wFileName = (wchar_t*)utf8ToUtf16(fileName->c_str());
  file = GooFile::open(wFileName);
  gfree(wFileName);
#else
   file = GooFile::open(fileName);
#endif
  if (file == nullptr) {
    // fopen() has failed.
    // Keep a copy of the errno returned by fopen so that it can be 
    // referred to later.
    fopenErrno = errno;
    error(errIO, -1, "Couldn't open file '{0:t}': {1:s}.", fileName, strerror(errno));
    errCode = errOpenFile;
    return;
  }

  // create stream
  str = new FileStream(file, 0, false, file->size(), Object(objNull));

  ok = setup(ownerPassword, userPassword);
}