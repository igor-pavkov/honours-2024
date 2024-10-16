void HtmlOutputDev::startPage(int pageNum, GfxState *state, XRef *xref) {
#if 0
  if (mode&&!xml){
    if (write){
      write=false;
      GooString* fname=Dirname(Docname);
      fname->append("image.log");
      if((tin=fopen(getFileNameFromPath(fname->c_str(),fname->getLength()),"w"))==NULL){
	printf("Error : can not open %s",fname);
	exit(1);
      }
      delete fname;
    // if(state->getRotation()!=0) 
    //  fprintf(tin,"ROTATE=%d rotate %d neg %d neg translate\n",state->getRotation(),state->getX1(),-state->getY1());
    // else 
      fprintf(tin,"ROTATE=%d neg %d neg translate\n",state->getX1(),state->getY1());  
    }
  }
#endif

  this->pageNum = pageNum;
  const std::string str = gbasename(Docname->c_str());
  pages->clear(); 
  if(!noframes)
  {
    if (fContentsFrame)
	{
      if (complexMode)
		fprintf(fContentsFrame,"<a href=\"%s-%d.html\"", str.c_str(), pageNum);
      else 
		fprintf(fContentsFrame,"<a href=\"%ss.html#%d\"", str.c_str(), pageNum);
      fprintf(fContentsFrame," target=\"contents\" >Page %d</a><br/>\n",pageNum);
    }
  }

  pages->pageWidth=static_cast<int>(state->getPageWidth());
  pages->pageHeight=static_cast<int>(state->getPageHeight());
} 