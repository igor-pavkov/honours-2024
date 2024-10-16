void HtmlPage::clear() {
  HtmlString *p1, *p2;

  if (curStr) {
    delete curStr;
    curStr = nullptr;
  }
  for (p1 = yxStrings; p1; p1 = p2) {
    p2 = p1->yxNext;
    delete p1;
  }
  yxStrings = nullptr;
  xyStrings = nullptr;
  yxCur1 = yxCur2 = nullptr;

  if( !noframes )
  {
      delete fonts;
      fonts=new HtmlFontAccu();
      fontsPageMarker = 0;
  }
  else
  {
      fontsPageMarker = fonts->size();
  }

  delete links;
  links=new HtmlLinks();
 

}