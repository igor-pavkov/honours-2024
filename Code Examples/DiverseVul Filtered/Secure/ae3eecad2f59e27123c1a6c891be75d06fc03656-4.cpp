static void PushRunlengthPacket(Image *image,const unsigned char *pixels,
  size_t *length,PixelPacket *pixel,IndexPacket *index)
{
  const unsigned char
    *p;

  p=pixels;
  if (image->storage_class == PseudoClass)
    {
      *index=(IndexPacket) 0;
      switch (image->depth)
      {
        case 32:
        {
          *index=ConstrainColormapIndex(image,((size_t) *p << 24) |
            ((size_t) *(p+1) << 16) | ((size_t) *(p+2) << 8) | (size_t) *(p+3));
          p+=4;
          break;
        }
        case 16:
        {
          *index=ConstrainColormapIndex(image,(*p << 8) | *(p+1));
          p+=2;
          break;
        }
        case 8:
        {
          *index=ConstrainColormapIndex(image,*p);
          p++;
          break;
        }
        default:
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      *pixel=image->colormap[(ssize_t) *index];
      switch (image->depth)
      {
        case 8:
        {
          unsigned char
            quantum;

          if (image->matte != MagickFalse)
            {
              p=PushCharPixel(p,&quantum);
              pixel->opacity=ScaleCharToQuantum(quantum);
            }
          break;
        }
        case 16:
        {
          unsigned short
            quantum;

          if (image->matte != MagickFalse)
            {
              p=PushShortPixel(MSBEndian,p,&quantum);
              pixel->opacity=(Quantum) (quantum >> (image->depth-
                MAGICKCORE_QUANTUM_DEPTH));
            }
          break;
        }
        case 32:
        {
          unsigned int
            quantum;

          if (image->matte != MagickFalse)
            {
              p=PushLongPixel(MSBEndian,p,&quantum);
              pixel->opacity=(Quantum) (quantum >> (image->depth-
                MAGICKCORE_QUANTUM_DEPTH));
            }
          break;
        }
        default:
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      *length=(size_t) (*p++)+1;
      return;
    }
  switch (image->depth)
  {
    case 8:
    {
      unsigned char
        quantum;

      p=PushCharPixel(p,&quantum);
      SetPixelRed(pixel,ScaleCharToQuantum(quantum));
      SetPixelGreen(pixel,ScaleCharToQuantum(quantum));
      SetPixelBlue(pixel,ScaleCharToQuantum(quantum));
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushCharPixel(p,&quantum);
          SetPixelGreen(pixel,ScaleCharToQuantum(quantum));
          p=PushCharPixel(p,&quantum);
          SetPixelBlue(pixel,ScaleCharToQuantum(quantum));
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushCharPixel(p,&quantum);
          SetPixelBlack(index,ScaleCharToQuantum(quantum));
        }
      if (image->matte != MagickFalse)
        {
          p=PushCharPixel(p,&quantum);
          SetPixelOpacity(pixel,ScaleCharToQuantum(quantum));
        }
      break;
    }
    case 16:
    {
      unsigned short
        quantum;

      p=PushShortPixel(MSBEndian,p,&quantum);
      SetPixelRed(pixel,quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH));
      SetPixelGreen(pixel,ScaleCharToQuantum((unsigned char) quantum));
      SetPixelBlue(pixel,ScaleCharToQuantum((unsigned char) quantum));
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          SetPixelGreen(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
          p=PushShortPixel(MSBEndian,p,&quantum);
          SetPixelBlue(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          SetPixelBlack(index,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      if (image->matte != MagickFalse)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          SetPixelOpacity(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      break;
    }
    case 32:
    {
      unsigned int
        quantum;

      p=PushLongPixel(MSBEndian,p,&quantum);
      SetPixelRed(pixel,quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH));
      SetPixelGreen(pixel,ScaleCharToQuantum(quantum));
      SetPixelBlue(pixel,ScaleCharToQuantum(quantum));
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          SetPixelGreen(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
          p=PushLongPixel(MSBEndian,p,&quantum);
          SetPixelBlue(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          SetPixelIndex(index,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      if (image->matte != MagickFalse)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          SetPixelOpacity(pixel,quantum >> (image->depth-
            MAGICKCORE_QUANTUM_DEPTH));
        }
      break;
    }
    default:
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
  }
  *length=(size_t) (*p++)+1;
}