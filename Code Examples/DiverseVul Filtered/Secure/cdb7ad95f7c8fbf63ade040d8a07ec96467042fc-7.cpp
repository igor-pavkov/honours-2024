void FoFiTrueType::convertToCIDType2(char *psName,
				     int *cidMap, int nCIDs,
				     GBool needVerticalMetrics,
				     FoFiOutputFunc outputFunc,
				     void *outputStream) {
  GooString *buf;
  int cid, maxUsedGlyph;
  GBool ok;
  int i, j, k;

  if (openTypeCFF) {
    return;
  }

  // write the header
  ok = gTrue;
  buf = GooString::format("%!PS-TrueTypeFont-{0:2g}\n",
			(double)getS32BE(0, &ok) / 65536.0);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;

  // begin the font dictionary
  (*outputFunc)(outputStream, "20 dict begin\n", 14);
  (*outputFunc)(outputStream, "/CIDFontName /", 14);
  (*outputFunc)(outputStream, psName, strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/CIDFontType 2 def\n", 19);
  (*outputFunc)(outputStream, "/FontType 42 def\n", 17);
  (*outputFunc)(outputStream, "/CIDSystemInfo 3 dict dup begin\n", 32);
  (*outputFunc)(outputStream, "  /Registry (Adobe) def\n", 24);
  (*outputFunc)(outputStream, "  /Ordering (Identity) def\n", 27);
  (*outputFunc)(outputStream, "  /Supplement 0 def\n", 20);
  (*outputFunc)(outputStream, "  end def\n", 10);
  (*outputFunc)(outputStream, "/GDBytes 2 def\n", 15);
  if (cidMap) {
    buf = GooString::format("/CIDCount {0:d} def\n", nCIDs);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    if (nCIDs > 32767) {
      (*outputFunc)(outputStream, "/CIDMap [", 9);
      for (i = 0; i < nCIDs; i += 32768 - 16) {
	(*outputFunc)(outputStream, "<\n", 2);
	for (j = 0; j < 32768 - 16 && i+j < nCIDs; j += 16) {
	  (*outputFunc)(outputStream, "  ", 2);
	  for (k = 0; k < 16 && i+j+k < nCIDs; ++k) {
	    cid = cidMap[i+j+k];
	    buf = GooString::format("{0:02x}{1:02x}",
				  (cid >> 8) & 0xff, cid & 0xff);
	    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	    delete buf;
	  }
	  (*outputFunc)(outputStream, "\n", 1);
	}
	(*outputFunc)(outputStream, "  >", 3);
      }
      (*outputFunc)(outputStream, "\n", 1);
      (*outputFunc)(outputStream, "] def\n", 6);
    } else {
      (*outputFunc)(outputStream, "/CIDMap <\n", 10);
      for (i = 0; i < nCIDs; i += 16) {
	(*outputFunc)(outputStream, "  ", 2);
	for (j = 0; j < 16 && i+j < nCIDs; ++j) {
	  cid = cidMap[i+j];
	  buf = GooString::format("{0:02x}{1:02x}",
				(cid >> 8) & 0xff, cid & 0xff);
	  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	  delete buf;
	}
	(*outputFunc)(outputStream, "\n", 1);
      }
      (*outputFunc)(outputStream, "> def\n", 6);
    }
  } else {
    // direct mapping - just fill the string(s) with s[i]=i
    buf = GooString::format("/CIDCount {0:d} def\n", nGlyphs);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    if (nGlyphs > 32767) {
      (*outputFunc)(outputStream, "/CIDMap [\n", 10);
      for (i = 0; i < nGlyphs; i += 32767) {
	j = nGlyphs - i < 32767 ? nGlyphs - i : 32767;
	buf = GooString::format("  {0:d} string 0 1 {1:d} {{\n", 2 * j, j - 1);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
	buf = GooString::format("    2 copy dup 2 mul exch {0:d} add -8 bitshift put\n",
			      i);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
	buf = GooString::format("    1 index exch dup 2 mul 1 add exch {0:d} add"
			      " 255 and put\n", i);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
	(*outputFunc)(outputStream, "  } for\n", 8);
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    } else {
      buf = GooString::format("/CIDMap {0:d} string\n", 2 * nGlyphs);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
      buf = GooString::format("  0 1 {0:d} {{\n", nGlyphs - 1);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
      (*outputFunc)(outputStream,
		    "    2 copy dup 2 mul exch -8 bitshift put\n", 42);
      (*outputFunc)(outputStream,
		    "    1 index exch dup 2 mul 1 add exch 255 and put\n", 50);
      (*outputFunc)(outputStream, "  } for\n", 8);
      (*outputFunc)(outputStream, "def\n", 4);
    }
  }
  (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  buf = GooString::format("/FontBBox [{0:d} {1:d} {2:d} {3:d}] def\n",
			bbox[0], bbox[1], bbox[2], bbox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/PaintType 0 def\n", 17);
  (*outputFunc)(outputStream, "/Encoding [] readonly def\n", 26);
  (*outputFunc)(outputStream, "/CharStrings 1 dict dup begin\n", 30);
  (*outputFunc)(outputStream, "  /.notdef 0 def\n", 17);
  (*outputFunc)(outputStream, "  end readonly def\n", 19);

  // write the guts of the dictionary
  cvtSfnts(outputFunc, outputStream, NULL, needVerticalMetrics, &maxUsedGlyph);

  // end the dictionary and define the font
  (*outputFunc)(outputStream,
		"CIDFontName currentdict end /CIDFont defineresource pop\n",
		56);
}