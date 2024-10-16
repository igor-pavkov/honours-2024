PDFDoc *PDFDoc::ErrorPDFDoc(int errorCode, const GooString *fileNameA)
{
  PDFDoc *doc = new PDFDoc();
  doc->errCode = errorCode;
  doc->fileName = fileNameA;

  return doc;
}