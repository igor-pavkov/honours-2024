int HtmlPage::dumpComplexHeaders(FILE * const file, FILE *& pageFile, int page) {
  GooString* tmp;

  if( !noframes )
  {
      GooString* pgNum=GooString::fromInt(page);
      tmp = new GooString(DocName);
      if (!singleHtml){
            tmp->append('-')->append(pgNum)->append(".html");
            pageFile = fopen(tmp->c_str(), "w");
      } else {
            tmp->append("-html")->append(".html");
            pageFile = fopen(tmp->c_str(), "a");
      }
      delete pgNum;
      if (!pageFile) {
	  error(errIO, -1, "Couldn't open html file '{0:t}'", tmp);
	  delete tmp;
	  return 1;
      } 

      if (!singleHtml)
        fprintf(pageFile,"%s\n<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"\" xml:lang=\"\">\n<head>\n<title>Page %d</title>\n\n", DOCTYPE, page);
      else
        fprintf(pageFile,"%s\n<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"\" xml:lang=\"\">\n<head>\n<title>%s</title>\n\n", DOCTYPE, tmp->c_str());

      delete tmp;

      GooString *htmlEncoding = HtmlOutputDev::mapEncodingToHtml(globalParams->getTextEncodingName());
      if (!singleHtml)
        fprintf(pageFile, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\"/>\n", htmlEncoding->c_str());
      else
        fprintf(pageFile, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\"/>\n <br/>\n", htmlEncoding->c_str());
      delete htmlEncoding;
  }
  else 
  {
      pageFile = file;
      fprintf(pageFile,"<!-- Page %d -->\n", page);
      fprintf(pageFile,"<a name=\"%d\"></a>\n", page);
  } 
  
  return 0;
}