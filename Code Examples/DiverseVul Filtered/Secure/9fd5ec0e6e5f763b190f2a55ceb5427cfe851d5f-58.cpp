static PDFSubtypeConformance pdfConformanceFromString(GooString *pdfSubtypeVersion) {
  const std::regex regex("PDF/(?:A|X|VT|E|UA)-[[:digit:]]([[:alpha:]]+)");
  std::smatch match;
  const std::string pdfsubver = pdfSubtypeVersion->toStr();
  PDFSubtypeConformance pdfConf = subtypeConfNone;

  // match contains the PDF conformance (A, B, G, N, P, PG or U)
  if (std::regex_search(pdfsubver, match, regex)) {
    GooString *conf = new GooString(match.str(1));
    // Convert to lowercase as the conformance may appear in both cases
    conf->lowerCase();
    if (conf->cmp("a")==0) {
      pdfConf = subtypeConfA;
    } else if (conf->cmp("b")==0) {
      pdfConf = subtypeConfB;
    } else if (conf->cmp("g")==0) {
      pdfConf = subtypeConfG;
    } else if (conf->cmp("n")==0) {
      pdfConf = subtypeConfN;
    } else if (conf->cmp("p")==0) {
      pdfConf = subtypeConfP;
    } else if (conf->cmp("pg")==0) {
      pdfConf = subtypeConfPG;
    } else if (conf->cmp("u")==0) {
      pdfConf = subtypeConfU;
    } else {
      pdfConf = subtypeConfNone;
    }
    delete conf;
  }

  return pdfConf;
}