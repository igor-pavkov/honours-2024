void HtmlOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
			      int width, int height, GfxImageColorMap *colorMap,
			      bool interpolate, int *maskColors, bool inlineImg) {

  if (ignore||(complexMode && !xml)) {
    OutputDev::drawImage(state, ref, str, width, height, colorMap, interpolate,
			 maskColors, inlineImg);
    return;
  }
  
  /*if( !globalParams->getErrQuiet() )
    printf("image stream of kind %d\n", str->getKind());*/
  // dump JPEG file
  if (dumpJPEG && str->getKind() == strDCT && (colorMap->getNumPixelComps() == 1 ||
	  colorMap->getNumPixelComps() == 3) && !inlineImg) {
    drawJpegImage(state, str);
  }
  else {
#ifdef ENABLE_LIBPNG
    drawPngImage(state, str, width, height, colorMap );
#else
    OutputDev::drawImage(state, ref, str, width, height, colorMap, interpolate,
                         maskColors, inlineImg);
#endif
  }
}