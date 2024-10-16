HtmlPage::HtmlPage(bool rawOrder) {
  this->rawOrder = rawOrder;
  curStr = nullptr;
  yxStrings = nullptr;
  xyStrings = nullptr;
  yxCur1 = yxCur2 = nullptr;
  fonts=new HtmlFontAccu();
  links=new HtmlLinks();
  imgList=new std::vector<HtmlImage*>();
  pageWidth=0;
  pageHeight=0;
  fontsPageMarker = 0;
  DocName=nullptr;
  firstPage = -1;
}