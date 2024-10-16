void HtmlPage::dumpComplex(FILE *file, int page, const std::vector<std::string>& backgroundImages) {
  FILE* pageFile;

  if( firstPage == -1 ) firstPage = page; 
  
  if (dumpComplexHeaders(file, pageFile, page)) { error(errIO, -1, "Couldn't write headers."); return; }
   
  fputs("<style type=\"text/css\">\n<!--\n",pageFile);
  fputs("\tp {margin: 0; padding: 0;}",pageFile);
  for(int i=fontsPageMarker;i!=fonts->size();i++) {
    GooString *fontCSStyle;
    if (!singleHtml)
         fontCSStyle = fonts->CSStyle(i);
    else
         fontCSStyle = fonts->CSStyle(i,page);
    fprintf(pageFile,"\t%s\n",fontCSStyle->c_str());
    delete fontCSStyle;
  }
 
  fputs("-->\n</style>\n",pageFile);
  
  if( !noframes )
  {  
      fputs("</head>\n<body bgcolor=\"#A0A0A0\" vlink=\"blue\" link=\"blue\">\n",pageFile); 
  }
  
  fprintf(pageFile,"<div id=\"page%d-div\" style=\"position:relative;width:%dpx;height:%dpx;\">\n",
      page, pageWidth, pageHeight);

  if(!ignore && (size_t) (page - firstPage) < backgroundImages.size())
  {
    fprintf(pageFile,
      "<img width=\"%d\" height=\"%d\" src=\"%s\" alt=\"background image\"/>\n",
      pageWidth, pageHeight, backgroundImages[page - firstPage].c_str());
  }
  
  for(HtmlString *tmp1=yxStrings;tmp1;tmp1=tmp1->yxNext){
    if (tmp1->htext){
      fprintf(pageFile,
	      "<p style=\"position:absolute;top:%dpx;left:%dpx;white-space:nowrap\" class=\"ft",
	      xoutRound(tmp1->yMin),
	      xoutRound(tmp1->xMin));
      if (!singleHtml) {
          fputc('0', pageFile);
      } else {
          fprintf(pageFile, "%d", page);
      }
      fprintf(pageFile,"%d\">", tmp1->fontpos);
      fputs(tmp1->htext->c_str(), pageFile);
      fputs("</p>\n", pageFile);
    }
  }

  fputs("</div>\n", pageFile);
  
  if( !noframes )
  {
      fputs("</body>\n</html>\n",pageFile);
      fclose(pageFile);
  }
}