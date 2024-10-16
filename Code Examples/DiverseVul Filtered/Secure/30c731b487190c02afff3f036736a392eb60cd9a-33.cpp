GooString* HtmlOutputDev::getLinkDest(AnnotLink *link){
  if (!link->getAction())
    return new GooString();
  switch(link->getAction()->getKind()) 
  {
      case actionGoTo:
	  {
	  GooString* file = new GooString(gbasename(Docname->c_str()));
	  int page=1;
	  LinkGoTo *ha=(LinkGoTo *)link->getAction();
	  LinkDest *dest=nullptr;
	  if (ha->getDest()!=nullptr)
	      dest=ha->getDest()->copy();
	  else if (ha->getNamedDest()!=nullptr)
	      dest=catalog->findDest(ha->getNamedDest());
	      
	  if (dest){ 
	      if (dest->isPageRef()){
		  Ref pageref=dest->getPageRef();
		  page=catalog->findPage(pageref.num,pageref.gen);
	      }
	      else {
		  page=dest->getPageNum();
	      }

	      delete dest;

	      GooString *str=GooString::fromInt(page);
	      /* 		complex 	simple
	       	frames		file-4.html	files.html#4
		noframes	file.html#4	file.html#4
	       */
	      if (noframes)
	      {
		  file->append(".html#");
		  file->append(str);
	      }
	      else
	      {
	      	if( complexMode ) 
		{
		    file->append("-");
		    file->append(str);
		    file->append(".html");
		}
		else
		{
		    file->append("s.html#");
		    file->append(str);
		}
	      }

	      if (printCommands) printf(" link to page %d ",page);
	      delete str;
	      return file;
	  }
	  else 
	  {
	      return new GooString();
	  }
	  }
      case actionGoToR:
	  {
	  LinkGoToR *ha=(LinkGoToR *) link->getAction();
	  LinkDest *dest=nullptr;
	  int page=1;
	  GooString *file=new GooString();
	  if (ha->getFileName()){
	      delete file;
	      file=new GooString(ha->getFileName()->c_str());
	  }
	  if (ha->getDest()!=nullptr)  dest=ha->getDest()->copy();
	  if (dest&&file){
	      if (!(dest->isPageRef()))  page=dest->getPageNum();
	      delete dest;

	      if (printCommands) printf(" link to page %d ",page);
	      if (printHtml){
		  const char *p=file->c_str()+file->getLength()-4;
		  if (!strcmp(p, ".pdf") || !strcmp(p, ".PDF")){
		      file->del(file->getLength()-4,4);
		      file->append(".html");
		  }
		  file->append('#');
		  GooString *pgNum = GooString::fromInt(page);
		  file->append(pgNum);
		  delete pgNum;
	      }
	  }
	  if (printCommands && file) printf("filename %s\n",file->c_str());
	  return file;
	  }
      case actionURI:
	  { 
	  LinkURI *ha=(LinkURI *) link->getAction();
	  GooString* file=new GooString(ha->getURI()->c_str());
	  // printf("uri : %s\n",file->c_str());
	  return file;
	  }
      case actionLaunch:
	  if (printHtml) {
	      LinkLaunch *ha=(LinkLaunch *) link->getAction();
	      GooString* file=new GooString(ha->getFileName()->c_str());
	      const char *p=file->c_str()+file->getLength()-4;
	      if (!strcmp(p, ".pdf") || !strcmp(p, ".PDF")){
		  file->del(file->getLength()-4,4);
		  file->append(".html");
	      }
	      if (printCommands) printf("filename %s",file->c_str());
    
	      return file;      
  
	  }
	  // fallthrough
      default:
	  return new GooString();
  }
}