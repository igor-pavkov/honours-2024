void PDFDoc::writeStream (Stream* str, OutStream* outStr)
{
  outStr->printf("stream\r\n");
  str->reset();
  for (int c=str->getChar(); c!= EOF; c=str->getChar()) {
    outStr->printf("%c", c);  
  }
  outStr->printf("\r\nendstream\r\n");
}