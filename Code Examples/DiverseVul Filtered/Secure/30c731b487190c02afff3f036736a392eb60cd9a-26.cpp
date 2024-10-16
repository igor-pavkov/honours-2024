void HtmlOutputDev::doFrame(int firstPage){
  GooString* fName=new GooString(Docname);
  GooString* htmlEncoding;
  fName->append(".html");

  if (!(fContentsFrame = fopen(fName->c_str(), "w"))){
    error(errIO, -1, "Couldn't open html file '{0:t}'", fName);
    delete fName;
    return;
  }
  
  delete fName;
    
  const std::string baseName = gbasename(Docname->c_str());
  fputs(DOCTYPE, fContentsFrame);
  fputs("\n<html>",fContentsFrame);
  fputs("\n<head>",fContentsFrame);
  fprintf(fContentsFrame,"\n<title>%s</title>",docTitle->c_str());
  htmlEncoding = mapEncodingToHtml(globalParams->getTextEncodingName());
  fprintf(fContentsFrame, "\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\"/>\n", htmlEncoding->c_str());
  dumpMetaVars(fContentsFrame);
  fprintf(fContentsFrame, "</head>\n");
  fputs("<frameset cols=\"100,*\">\n",fContentsFrame);
  fprintf(fContentsFrame,"<frame name=\"links\" src=\"%s_ind.html\"/>\n", baseName.c_str());
  fputs("<frame name=\"contents\" src=",fContentsFrame); 
  if (complexMode) 
      fprintf(fContentsFrame,"\"%s-%d.html\"", baseName.c_str(), firstPage);
  else
      fprintf(fContentsFrame,"\"%ss.html\"", baseName.c_str());
  
  fputs("/>\n</frameset>\n</html>\n",fContentsFrame);
 
  delete htmlEncoding;
  fclose(fContentsFrame);  
}