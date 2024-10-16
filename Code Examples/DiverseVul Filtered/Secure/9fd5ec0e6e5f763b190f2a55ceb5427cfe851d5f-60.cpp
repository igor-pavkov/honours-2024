void PDFDoc::writeString (const GooString* s, OutStream* outStr, const unsigned char *fileKey,
                          CryptAlgorithm encAlgorithm, int keyLength, int objNum, int objGen)
{
  // Encrypt string if encryption is enabled
  GooString *sEnc = nullptr;
  if (fileKey) {
    EncryptStream *enc = new EncryptStream(new MemStream(s->c_str(), 0, s->getLength(), Object(objNull)),
                                           fileKey, encAlgorithm, keyLength, objNum, objGen);
    sEnc = new GooString();
    int c;
    enc->reset();
    while ((c = enc->getChar()) != EOF) {
      sEnc->append((char)c);
    }

    delete enc;
    s = sEnc;
  }

  // Write data
  if (s->hasUnicodeMarker()) {
    //unicode string don't necessary end with \0
    const char* c = s->c_str();
    outStr->printf("(");
    for(int i=0; i<s->getLength(); i++) {
      char unescaped = *(c+i)&0x000000ff;
      //escape if needed
      if (unescaped == '(' || unescaped == ')' || unescaped == '\\')
        outStr->printf("%c", '\\');
      outStr->printf("%c", unescaped);
    }
    outStr->printf(") ");
  } else {
    const char* c = s->c_str();
    outStr->printf("(");
    for(int i=0; i<s->getLength(); i++) {
      char unescaped = *(c+i)&0x000000ff;
      //escape if needed
      if (unescaped == '\r')
        outStr->printf("\\r");
      else if (unescaped == '\n')
        outStr->printf("\\n");
      else {
        if (unescaped == '(' || unescaped == ')' || unescaped == '\\') {
          outStr->printf("%c", '\\');
        }
        outStr->printf("%c", unescaped);
      }
    }
    outStr->printf(") ");
  }

  delete sEnc;
}