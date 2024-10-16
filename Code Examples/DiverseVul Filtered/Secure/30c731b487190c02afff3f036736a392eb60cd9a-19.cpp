void HtmlPage::beginString(GfxState *state, const GooString *s) {
  curStr = new HtmlString(state, fontSize, fonts);
}