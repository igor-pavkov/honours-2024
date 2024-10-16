Object PDFDoc::createTrailerDict(int uxrefSize, bool incrUpdate, Goffset startxRef,
                                Ref *root, XRef *xRef, const char *fileName, Goffset fileSize)
{
  Dict *trailerDict = new Dict(xRef);
  trailerDict->set("Size", Object(uxrefSize));

  //build a new ID, as recommended in the reference, uses:
  // - current time
  // - file name
  // - file size
  // - values of entry in information dictionnary
  GooString message;
  char buffer[256];
  sprintf(buffer, "%i", (int)time(nullptr));
  message.append(buffer);

  if (fileName)
    message.append(fileName);

  sprintf(buffer, "%lli", (long long)fileSize);
  message.append(buffer);

  //info dict -- only use text string
  if (!xRef->getTrailerDict()->isNone()) {
    Object docInfo = xRef->getDocInfo();
    if (docInfo.isDict()) {
      for(int i=0; i<docInfo.getDict()->getLength(); i++) {
        Object obj2 = docInfo.getDict()->getVal(i);
        if (obj2.isString()) {
          message.append(obj2.getString());
        }
      }
    }
  }

  bool hasEncrypt = false;
  if (!xRef->getTrailerDict()->isNone()) {
    Object obj2 = xRef->getTrailerDict()->dictLookupNF("Encrypt").copy();
    if (!obj2.isNull()) {
      trailerDict->set("Encrypt", std::move(obj2));
      hasEncrypt = true;
    }
  }

  //calculate md5 digest
  unsigned char digest[16];
  md5((unsigned char*)message.c_str(), message.getLength(), digest);

  //create ID array
  // In case of encrypted files, the ID must not be changed because it's used to calculate the key
  if (incrUpdate || hasEncrypt) {
    //only update the second part of the array
    Object obj4  = xRef->getTrailerDict()->getDict()->lookup("ID");
    if (!obj4.isArray()) {
      error(errSyntaxWarning, -1, "PDFDoc::createTrailerDict original file's ID entry isn't an array. Trying to continue");
    } else {
      Array *array = new Array(xRef);
      //Get the first part of the ID
      array->add(obj4.arrayGet(0));
      array->add(Object(new GooString((const char*)digest, 16)));
      trailerDict->set("ID", Object(array));
    }
  } else {
    //new file => same values for the two identifiers
    Array *array = new Array(xRef);
    array->add(Object(new GooString((const char*)digest, 16)));
    array->add(Object(new GooString((const char*)digest, 16)));
    trailerDict->set("ID", Object(array));
  }

  trailerDict->set("Root", Object(*root));

  if (incrUpdate) { 
    trailerDict->set("Prev", Object(startxRef));
  }
  
  if (!xRef->getTrailerDict()->isNone()) {
    Object obj5 = xRef->getDocInfoNF();
    if (!obj5.isNull()) {
      trailerDict->set("Info", std::move(obj5));
    }
  }

  return Object(trailerDict);
}