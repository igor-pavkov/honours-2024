void PDFDoc::writeRawStream (Stream* str, OutStream* outStr)
{
  Object obj1 = str->getDict()->lookup("Length");
  if (!obj1.isInt() && !obj1.isInt64()) {
    error (errSyntaxError, -1, "PDFDoc::writeRawStream, no Length in stream dict");
    return;
  }

  Goffset length;
  if (obj1.isInt())
    length = obj1.getInt();
  else
    length = obj1.getInt64();

  outStr->printf("stream\r\n");
  str->unfilteredReset();
  for (Goffset i = 0; i < length; i++) {
    int c = str->getUnfilteredChar();
    if (unlikely(c == EOF)) {
      error (errSyntaxError, -1, "PDFDoc::writeRawStream: EOF reading stream");
      break;
    }
    outStr->printf("%c", c);  
  }
  str->reset();
  outStr->printf("\r\nendstream\r\n");
}