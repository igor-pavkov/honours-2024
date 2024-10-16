static void ExportCbYCrYQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const Quantum *magick_restrict p,
  unsigned char *magick_restrict q,ExceptionInfo *exception)
{
  Quantum
    cbcr[4];

  ssize_t
    i,
    x;

  unsigned int
    pixel;

  size_t
    quantum;

  ssize_t
    n;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  n=0;
  quantum=0;
  switch (quantum_info->depth)
  {
    case 10:
    {
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) number_pixels; x+=2)
          {
            for (i=0; i < 4; i++)
            {
              switch (n % 3)
              {
                case 0:
                {
                  quantum=GetPixelRed(image,p);
                  break;
                }
                case 1:
                {
                  quantum=GetPixelGreen(image,p);
                  break;
                }
                case 2:
                {
                  quantum=GetPixelBlue(image,p);
                  break;
                }
              }
              cbcr[i]=(Quantum) quantum;
              n++;
            }
            pixel=(unsigned int) ((size_t) (cbcr[1]) << 22 | (size_t)
              (cbcr[0]) << 12 | (size_t) (cbcr[2]) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            pixel=(unsigned int) ((size_t) (cbcr[3]) << 22 | (size_t)
              (cbcr[0]) << 12 | (size_t) (cbcr[2]) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      break;
    }
    default:
    {
      QuantumAny
        range;

      for (x=0; x < (ssize_t) number_pixels; x+=2)
      {
        for (i=0; i < 4; i++)
        {
          switch (n % 3)
          {
            case 0:
            {
              quantum=GetPixelRed(image,p);
              break;
            }
            case 1:
            {
              quantum=GetPixelGreen(image,p);
              break;
            }
            case 2:
            {
              quantum=GetPixelBlue(image,p);
              break;
            }
          }
          cbcr[i]=(Quantum) quantum;
          n++;
        }
        range=GetQuantumRange(quantum_info->depth);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[1],range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[0],range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[2],range),q);
        p+=GetPixelChannels(image);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[3],range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[0],range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(cbcr[2],range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}