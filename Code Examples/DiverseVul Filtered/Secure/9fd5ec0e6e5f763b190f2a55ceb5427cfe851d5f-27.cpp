PDFDoc::PDFDoc(wchar_t *fileNameA, int fileNameLen, GooString *ownerPassword,
	       GooString *userPassword, void *guiDataA) {
  OSVERSIONINFO version;
  int i;

  init();

  guiData = guiDataA;

  // save both Unicode and 8-bit copies of the file name
  GooString *fileNameG = new GooString();
  fileNameU = (wchar_t *)gmallocn(fileNameLen + 1, sizeof(wchar_t));
  for (i = 0; i < fileNameLen; ++i) {
    fileNameG->append((char)fileNameA[i]);
    fileNameU[i] = fileNameA[i];
  }
  fileName = fileNameG;
  fileNameU[fileNameLen] = L'\0';
  
  // try to open file
  // NB: _wfopen is only available in NT
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    file = GooFile::open(fileNameU);
  } else {
    file = GooFile::open(fileName);
  }
  if (!file) {
    error(errIO, -1, "Couldn't open file '{0:t}'", fileName);
    errCode = errOpenFile;
    return;
  }

  // create stream
  str = new FileStream(file, 0, false, file->size(), Object(objNull));

  ok = setup(ownerPassword, userPassword);
}