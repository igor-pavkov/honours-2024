HtmlString::~HtmlString() {
  gfree(text);
  delete htext;
  gfree(xRight);
}