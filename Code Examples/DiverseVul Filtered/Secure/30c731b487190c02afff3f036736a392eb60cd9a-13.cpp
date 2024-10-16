void HtmlOutputDev::doProcessLink(AnnotLink* link){
  double _x1,_y1,_x2,_y2;
  int x1,y1,x2,y2;

  link->getRect(&_x1,&_y1,&_x2,&_y2);
  cvtUserToDev(_x1,_y1,&x1,&y1);
  
  cvtUserToDev(_x2,_y2,&x2,&y2); 


  GooString* _dest=getLinkDest(link);
  HtmlLink t((double) x1,(double) y2,(double) x2,(double) y1,_dest);
  pages->AddLink(t);
  delete _dest;
}