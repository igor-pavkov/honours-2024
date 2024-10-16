Goffset PDFDoc::getStartXRef(bool tryingToReconstruct)
{
  if (startXRefPos == -1) {

    if (isLinearized(tryingToReconstruct)) {
      char buf[linearizationSearchSize+1];
      int c, n, i;

      str->setPos(0);
      for (n = 0; n < linearizationSearchSize; ++n) {
        if ((c = str->getChar()) == EOF) {
          break;
        }
        buf[n] = c;
      }
      buf[n] = '\0';

      // find end of first obj (linearization dictionary)
      startXRefPos = 0;
      for (i = 0; i < n; i++) {
        if (!strncmp("endobj", &buf[i], 6)) {
	  i += 6;
	  //skip whitespace 
	  while (buf[i] && Lexer::isSpace(buf[i])) ++i;
	  startXRefPos = i;
	  break;
        }
      }
    } else {
      char buf[xrefSearchSize+1];
      const char *p;
      int c, n, i;

      // read last xrefSearchSize bytes
      int segnum = 0;
      int maxXRefSearch = 24576;
      if (str->getLength() < maxXRefSearch) maxXRefSearch = str->getLength();
      for (; (xrefSearchSize - 16) * segnum < maxXRefSearch; segnum++) {
        str->setPos((xrefSearchSize - 16) * segnum + xrefSearchSize, -1);
        for (n = 0; n < xrefSearchSize; ++n) {
          if ((c = str->getChar()) == EOF) {
            break;
          }
          buf[n] = c;
        }
        buf[n] = '\0';

        // find startxref
        for (i = n - 9; i >= 0; --i) {
          if (!strncmp(&buf[i], "startxref", 9)) {
            break;
          }
        }
        if (i < 0) {
          startXRefPos = 0;
        } else {
          for (p = &buf[i + 9]; isspace(*p); ++p);
          startXRefPos = strToLongLong(p);
          break;
        }
      }
    }

  }

  return startXRefPos;
}