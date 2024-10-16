void HtmlPage::addImage(GooString *fname, GfxState *state) {
  HtmlImage *img = new HtmlImage(fname, state);
  imgList->push_back(img);
}