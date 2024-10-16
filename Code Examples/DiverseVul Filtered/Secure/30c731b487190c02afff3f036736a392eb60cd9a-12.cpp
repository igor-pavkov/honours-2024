void HtmlOutputDev::drawPngImage(GfxState *state, Stream *str, int width, int height,
                                 GfxImageColorMap *colorMap, bool isMask)
{
#ifdef ENABLE_LIBPNG
  FILE *f1;
  InMemoryFile ims;

  if (!colorMap && !isMask) {
    error(errInternal, -1, "Can't have color image without a color map");
    return;
  }

  // open the image file
  GooString *fName=createImageFileName("png");
  f1 = dataUrls ? ims.open("wb") : fopen(fName->c_str(), "wb");
  if (!f1) {
    error(errIO, -1, "Couldn't open image file '{0:t}'", fName);
    delete fName;
    return;
  }

  PNGWriter *writer = new PNGWriter( isMask ? PNGWriter::MONOCHROME : PNGWriter::RGB );
  // TODO can we calculate the resolution of the image?
  if (!writer->init(f1, width, height, 72, 72)) {
    error(errInternal, -1, "Can't init PNG for image '{0:t}'", fName);
    delete writer;
    fclose(f1);
    return;
  }

  if (!isMask) {
    unsigned char *p;
    GfxRGB rgb;
    png_byte *row = (png_byte *) gmalloc(3 * width);   // 3 bytes/pixel: RGB
    png_bytep *row_pointer= &row;

    // Initialize the image stream
    ImageStream *imgStr = new ImageStream(str, width,
                        colorMap->getNumPixelComps(), colorMap->getBits());
    imgStr->reset();

    // For each line...
    for (int y = 0; y < height; y++) {

      // Convert into a PNG row
      p = imgStr->getLine();
      if (!p) {
        error(errIO, -1, "Failed to read PNG. '{0:t}' will be incorrect", fName);
        delete fName;
        gfree(row);
        delete writer;
        delete imgStr;
        fclose(f1);
        return;
      }
      for (int x = 0; x < width; x++) {
        colorMap->getRGB(p, &rgb);
        // Write the RGB pixels into the row
        row[3*x]= colToByte(rgb.r);
        row[3*x+1]= colToByte(rgb.g);
        row[3*x+2]= colToByte(rgb.b);
        p += colorMap->getNumPixelComps();
      }

      if (!writer->writeRow(row_pointer)) {
        error(errIO, -1, "Failed to write into PNG '{0:t}'", fName);
        delete writer;
        delete imgStr;
        fclose(f1);
        return;
      }
    }
    gfree(row);
    imgStr->close();
    delete imgStr;
  }
  else { // isMask == true
    int size = (width + 7)/8;

    // PDF masks use 0 = draw current color, 1 = leave unchanged.
    // We invert this to provide the standard interpretation of alpha
    // (0 = transparent, 1 = opaque). If the colorMap already inverts
    // the mask we leave the data unchanged.
    int invert_bits = 0xff;
    if (colorMap) {
      GfxGray gray;
      unsigned char zero[gfxColorMaxComps];
      memset(zero, 0, sizeof(zero));
      colorMap->getGray(zero, &gray);
      if (colToByte(gray) == 0)
        invert_bits = 0x00;
    }

    str->reset();
    unsigned char *png_row = (unsigned char *)gmalloc(size);

    for (int ri = 0; ri < height; ++ri)
    {
      for(int i = 0; i < size; i++)
        png_row[i] = str->getChar() ^ invert_bits;

      if (!writer->writeRow( &png_row ))
      {
        error(errIO, -1, "Failed to write into PNG '{0:t}'", fName);
        delete writer;
        fclose(f1);
        gfree(png_row);
        return;
      }
    }
    str->close();
    gfree(png_row);
  }

  str->close();

  writer->close();
  delete writer;
  fclose(f1);

  if (dataUrls) {
    delete fName;
    fName = new GooString(std::string("data:image/png;base64,") + gbase64Encode(ims.getBuffer()));
  }
  pages->addImage(fName, state);
#else
  return;
#endif
}