static unsigned char *PopRunlengthPacket(Image *image,unsigned char *pixels,
  size_t length,PixelPacket pixel,IndexPacket index)
{
  if (image->storage_class != DirectClass)
    {
      unsigned int
        value;

      value=(unsigned int) index;
      switch (image->depth)
      {
        case 32:
        {
          *pixels++=(unsigned char) (value >> 24);
          *pixels++=(unsigned char) (value >> 16);
        }
        case 16:
          *pixels++=(unsigned char) (value >> 8);
        case 8:
        {
          *pixels++=(unsigned char) value;
          break;
        }
        default:
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      switch (image->depth)
      {
        case 32:
        {
          unsigned int
            value;

          if (image->matte != MagickFalse)
            {
              value=ScaleQuantumToLong(pixel.opacity);
              pixels=PopLongPixel(MSBEndian,value,pixels);
            }
          break;
        }
        case 16:
        {
          unsigned short
            value;

          if (image->matte != MagickFalse)
            {
              value=ScaleQuantumToShort(pixel.opacity);
              pixels=PopShortPixel(MSBEndian,value,pixels);
            }
          break;
        }
        case 8:
        {
          unsigned char
            value;

          if (image->matte != MagickFalse)
            {
              value=(unsigned char) ScaleQuantumToChar(pixel.opacity);
              pixels=PopCharPixel(value,pixels);
            }
          break;
        }
        default:
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      *pixels++=(unsigned char) length;
      return(pixels);
    }
  switch (image->depth)
  {
    case 32:
    {
      unsigned int
        value;

      value=ScaleQuantumToLong(pixel.red);
      pixels=PopLongPixel(MSBEndian,value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=ScaleQuantumToLong(pixel.green);
          pixels=PopLongPixel(MSBEndian,value,pixels);
          value=ScaleQuantumToLong(pixel.blue);
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=ScaleQuantumToLong(index);
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      if (image->matte != MagickFalse)
        {
          value=ScaleQuantumToLong(pixel.opacity);
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      break;
    }
    case 16:
    {
      unsigned short
        value;

      value=ScaleQuantumToShort(pixel.red);
      pixels=PopShortPixel(MSBEndian,value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=ScaleQuantumToShort(pixel.green);
          pixels=PopShortPixel(MSBEndian,value,pixels);
          value=ScaleQuantumToShort(pixel.blue);
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=ScaleQuantumToShort(index);
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      if (image->matte != MagickFalse)
        {
          value=ScaleQuantumToShort(pixel.opacity);
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      break;
    }
    case 8:
    {
      unsigned char
        value;

      value=(unsigned char) ScaleQuantumToChar(pixel.red);
      pixels=PopCharPixel(value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=(unsigned char) ScaleQuantumToChar(pixel.green);
          pixels=PopCharPixel(value,pixels);
          value=(unsigned char) ScaleQuantumToChar(pixel.blue);
          pixels=PopCharPixel(value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=(unsigned char) ScaleQuantumToChar(index);
          pixels=PopCharPixel(value,pixels);
        }
      if (image->matte != MagickFalse)
        {
          value=(unsigned char) ScaleQuantumToChar(pixel.opacity);
          pixels=PopCharPixel(value,pixels);
        }
      break;
    }
    default:
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
  }
  *pixels++=(unsigned char) length;
  return(pixels);
}