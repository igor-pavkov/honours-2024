static PDFSubtypePart pdfPartFromString(PDFSubtype subtype, GooString *pdfSubtypeVersion) {
  const std::regex regex("PDF/(?:A|X|VT|E|UA)-([[:digit:]])(?:[[:alpha:]]{1,2})?:?([[:digit:]]{4})?");
  std::smatch match;
  std::string pdfsubver = pdfSubtypeVersion->toStr();
  PDFSubtypePart subtypePart = subtypePartNone;

  if (std::regex_search(pdfsubver, match, regex)) {
    int date = 0;
    const int part = std::stoi(match.str(1));

    if (match[2].matched) {
      date = std::stoi(match.str(2));
    }

    switch (subtype) {
      case subtypePDFX:
        switch (part) {
          case 1:
            switch (date) {
              case 2001:
              default:
                subtypePart = subtypePart1;
                break;
              case 2003:
                subtypePart = subtypePart4;
                break;
            }
            break;
          case 2:
            subtypePart = subtypePart5;
            break;
          case 3:
            switch (date) {
              case 2002:
              default:
                subtypePart = subtypePart3;
                break;
              case 2003:
                subtypePart = subtypePart6;
                break;
            }
            break;
          case 4:
            subtypePart = subtypePart7;
            break;
          case 5:
            subtypePart = subtypePart8;
            break;
        }
        break;
      default:
        subtypePart = (PDFSubtypePart)part;
        break;

    }
  }

  return subtypePart;
}