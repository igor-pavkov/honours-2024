void HtmlOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				  int width, int height, bool invert,
				  bool interpolate, bool inlineImg) {

  if (ignore||(complexMode && !xml)) {
    OutputDev::drawImageMask(state, ref, str, width, height, invert, interpolate, inlineImg);
    return;
  }
  
  // dump JPEG file
  if (dumpJPEG  && str->getKind() == strDCT) {
    drawJpegImage(state, str);
  }
  else {
#ifdef ENABLE_LIBPNG
    drawPngImage(state, str, width, height, nullptr, true);
#else
    OutputDev::drawImageMask(state, ref, str, width, height, invert, interpolate, inlineImg);
#endif
  }
}