void PDFDoc::writeObject (Object* obj, OutStream* outStr, XRef *xRef, unsigned int numOffset, unsigned char *fileKey,
                          CryptAlgorithm encAlgorithm, int keyLength, int objNum, int objGen, std::set<Dict*> *alreadyWrittenDicts)
{
  Array *array;

  switch (obj->getType()) {
    case objBool:
      outStr->printf("%s ", obj->getBool()?"true":"false");
      break;
    case objInt:
      outStr->printf("%i ", obj->getInt());
      break;
    case objInt64:
      outStr->printf("%lli ", obj->getInt64());
      break;
    case objReal:
    {
      GooString s;
      s.appendf("{0:.10g}", obj->getReal());
      outStr->printf("%s ", s.c_str());
      break;
    }
    case objString:
      writeString(obj->getString(), outStr, fileKey, encAlgorithm, keyLength, objNum, objGen);
      break;
    case objName:
    {
      GooString name(obj->getName());
      GooString *nameToPrint = name.sanitizedName(false /* non ps mode */);
      outStr->printf("/%s ", nameToPrint->c_str());
      delete nameToPrint;
      break;
    }
    case objNull:
      outStr->printf( "null ");
      break;
    case objArray:
      array = obj->getArray();
      outStr->printf("[");
      for (int i=0; i<array->getLength(); i++) {
	Object obj1 = array->getNF(i).copy();
        writeObject(&obj1, outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen);
      }
      outStr->printf("] ");
      break;
    case objDict:
      writeDictionnary (obj->getDict(), outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen, alreadyWrittenDicts);
      break;
    case objStream: 
      {
        //We can't modify stream with the current implementation (no write functions in Stream API)
        // => the only type of streams which that have been modified are internal streams (=strWeird)
        Stream *stream = obj->getStream();
        if (stream->getKind() == strWeird || stream->getKind() == strCrypt) {
          //we write the stream unencoded => TODO: write stream encoder

          // Encrypt stream
          EncryptStream *encStream = nullptr;
          bool removeFilter = true;
          if (stream->getKind() == strWeird && fileKey) {
            Object filter = stream->getDict()->lookup("Filter");
            if (!filter.isName("Crypt")) {
              if (filter.isArray()) {
                for (int i = 0; i < filter.arrayGetLength(); i++) {
                  Object filterEle = filter.arrayGet(i);
                  if (filterEle.isName("Crypt")) {
                    removeFilter = false;
                    break;
                  }
                }
                if (removeFilter) {
                  encStream = new EncryptStream(stream, fileKey, encAlgorithm, keyLength, objNum, objGen);
                  encStream->setAutoDelete(false);
                  stream = encStream;
                }
              } else {
                encStream = new EncryptStream(stream, fileKey, encAlgorithm, keyLength, objNum, objGen);
                encStream->setAutoDelete(false);
                stream = encStream;
              }
            } else {
              removeFilter = false;
            }
          } else if (fileKey != nullptr) { // Encrypt stream
            encStream = new EncryptStream(stream, fileKey, encAlgorithm, keyLength, objNum, objGen);
            encStream->setAutoDelete(false);
            stream = encStream;
          }

          stream->reset();
          //recalculate stream length
          Goffset tmp = 0;
          for (int c=stream->getChar(); c!=EOF; c=stream->getChar()) {
            tmp++;
          }
          stream->getDict()->set("Length", Object(tmp));

          //Remove Stream encoding
          if (removeFilter) {
            stream->getDict()->remove("Filter");
          }
          stream->getDict()->remove("DecodeParms");

          writeDictionnary (stream->getDict(),outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen, alreadyWrittenDicts);
          writeStream (stream,outStr);
          delete encStream;
        } else if (fileKey != nullptr && stream->getKind() == strFile && static_cast<FileStream*>(stream)->getNeedsEncryptionOnSave()) {
          EncryptStream *encStream = new EncryptStream(stream, fileKey, encAlgorithm, keyLength, objNum, objGen);
          encStream->setAutoDelete(false);
          writeDictionnary (encStream->getDict(), outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen, alreadyWrittenDicts);
          writeStream (encStream, outStr);
          delete encStream;
        } else {
          //raw stream copy
          FilterStream *fs = dynamic_cast<FilterStream*>(stream);
          if (fs) {
            BaseStream *bs = fs->getBaseStream();
            if (bs) {
              Goffset streamEnd;
                if (xRef->getStreamEnd(bs->getStart(), &streamEnd)) {
                  Goffset val = streamEnd - bs->getStart();
                  stream->getDict()->set("Length", Object(val));
                }
              }
          }
          writeDictionnary (stream->getDict(), outStr, xRef, numOffset, fileKey, encAlgorithm, keyLength, objNum, objGen, alreadyWrittenDicts);
          writeRawStream (stream, outStr);
        }
        break;
      }
    case objRef:
      outStr->printf("%i %i R ", obj->getRef().num + numOffset, obj->getRef().gen);
      break;
    case objCmd:
      outStr->printf("%s\n", obj->getCmd());
      break;
    case objError:
      outStr->printf("error\r\n");
      break;
    case objEOF:
      outStr->printf("eof\r\n");
      break;
    case objNone:
      outStr->printf("none\r\n");
      break;
    default:
      error(errUnimplemented, -1,"Unhandled objType : {0:d}, please report a bug with a testcase\r\n", obj->getType());
      break;
  }
}