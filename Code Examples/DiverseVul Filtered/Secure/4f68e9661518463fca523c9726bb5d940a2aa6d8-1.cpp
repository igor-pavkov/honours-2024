static inline void WriteTGAPixel(Image *image,TGAImageType image_type,
  const Quantum *p,const QuantumAny range,const double midpoint)
{
  if (image_type == TGAColormap || image_type == TGARLEColormap)
    (void) WriteBlobByte(image,(unsigned char) GetPixelIndex(image,p));
  else
    {
      if (image_type == TGAMonochrome || image_type == TGARLEMonochrome)
        (void) WriteBlobByte(image,ScaleQuantumToChar(ClampToQuantum(
          GetPixelLuma(image,p))));
      else
        if (image->depth == 5)
          {
            unsigned char
              green,
              value;

            green=(unsigned char) ScaleQuantumToAny(GetPixelGreen(image,p),
              range);
            value=((unsigned char) ScaleQuantumToAny(GetPixelBlue(image,p),
              range)) | ((green & 0x07) << 5);
            (void) WriteBlobByte(image,value);
            value=(((image->alpha_trait != UndefinedPixelTrait) &&
              ((double) GetPixelAlpha(image,p) > midpoint)) ? 0x80 : 0) |
              ((unsigned char) ScaleQuantumToAny(GetPixelRed(image,p),range) <<
              2) | ((green & 0x18) >> 3);
            (void) WriteBlobByte(image,value);
          }
        else
          {
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              GetPixelBlue(image,p)));
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              GetPixelGreen(image,p)));
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              GetPixelRed(image,p)));
            if (image->alpha_trait != UndefinedPixelTrait)
              (void) WriteBlobByte(image,ScaleQuantumToChar(
                GetPixelAlpha(image,p)));
          }
    }
}