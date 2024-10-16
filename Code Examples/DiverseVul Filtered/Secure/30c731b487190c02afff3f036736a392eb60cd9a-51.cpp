void HtmlOutputDev::drawJpegImage(GfxState *state, Stream *str)
{
  InMemoryFile ims;
  FILE *f1 = nullptr;
  int c;

  // open the image file
  GooString *fName = createImageFileName("jpg");
  f1 = dataUrls ? ims.open("wb") : fopen(fName->c_str(), "wb");
  if (!f1) {
    error(errIO, -1, "Couldn't open image file '{0:t}'", fName);
    delete fName;
    return;
  }

  // initialize stream
  str = str->getNextStream();
  str->reset();

  // copy the stream
  while ((c = str->getChar()) != EOF)
    fputc(c, f1);

  fclose(f1);

  if (dataUrls) {
    delete fName;
    fName = new GooString(std::string("data:image/jpeg;base64,") + gbase64Encode(ims.getBuffer()));
  }
  pages->addImage(fName, state);
}