void HtmlOutputDev::endPage() {
  Links *linksList = docPage->getLinks();
  for (int i = 0; i < linksList->getNumLinks(); ++i)
  {
      doProcessLink(linksList->getLink(i));
  }
  delete linksList;

  pages->conv();
  pages->coalesce();
  pages->dump(page, pageNum, backgroundImages);
  
  // I don't yet know what to do in the case when there are pages of different
  // sizes and we want complex output: running ghostscript many times 
  // seems very inefficient. So for now I'll just use last page's size
  maxPageWidth = pages->pageWidth;
  maxPageHeight = pages->pageHeight;
  
  //if(!noframes&&!xml) fputs("<br/>\n", fContentsFrame);
  if(!stout && !globalParams->getErrQuiet()) printf("Page-%d\n",(pageNum));
}