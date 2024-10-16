HtmlPage::~HtmlPage() {
  clear();
  delete DocName;
  delete fonts;
  delete links;
  for (auto entry : *imgList) {
    delete entry;
  }
  delete imgList;
}