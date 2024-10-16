void PDFDoc::extractPDFSubtype() {
  pdfSubtype = subtypeNull;
  pdfPart = subtypePartNull;
  pdfConformance = subtypeConfNull;

  GooString *pdfSubtypeVersion = nullptr;
  // Find PDF InfoDict subtype key if any
  if ((pdfSubtypeVersion = getDocInfoStringEntry("GTS_PDFA1Version"))) {
    pdfSubtype = subtypePDFA;
  } else if ((pdfSubtypeVersion = getDocInfoStringEntry("GTS_PDFEVersion"))) {
    pdfSubtype = subtypePDFE;
  } else if ((pdfSubtypeVersion = getDocInfoStringEntry("GTS_PDFUAVersion"))) {
    pdfSubtype = subtypePDFUA;
  } else if ((pdfSubtypeVersion = getDocInfoStringEntry("GTS_PDFVTVersion"))) {
    pdfSubtype = subtypePDFVT;
  } else if ((pdfSubtypeVersion = getDocInfoStringEntry("GTS_PDFXVersion"))) {
    pdfSubtype = subtypePDFX;
  } else {
    pdfSubtype = subtypeNone;
    pdfPart = subtypePartNone;
    pdfConformance = subtypeConfNone;
    return;
  }

  // Extract part from version string
  pdfPart = pdfPartFromString(pdfSubtype, pdfSubtypeVersion);

  // Extract conformance from version string
  pdfConformance = pdfConformanceFromString(pdfSubtypeVersion);

  delete pdfSubtypeVersion;
}