static size_t GetBytesPerRow(const size_t columns,
  const size_t samples_per_pixel,const size_t bits_per_pixel,
  const MagickBooleanType pad)
{
  size_t
    bytes_per_row;

  switch (bits_per_pixel)
  {
    case 1:
    {
      bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+31)/
        32);
      break;
    }
    case 8:
    default:
    {
      bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+31)/
        32);
      break;
    }
    case 10:
    {
      if (pad == MagickFalse)
        {
          bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+
            31)/32);
          break;
        }
      bytes_per_row=4*(((size_t) (32*((samples_per_pixel*columns+2)/3))+31)/32);
      break;
    }
    case 12:
    {
      if (pad == MagickFalse)
        {
          bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+
            31)/32);
          break;
        }
      bytes_per_row=2*(((size_t) (16*samples_per_pixel*columns)+15)/16);
      break;
    }
    case 16:
    {
      if (pad == MagickFalse)
        {
          bytes_per_row=2*(((size_t) samples_per_pixel*columns*bits_per_pixel+
            15)/16);
          break;
        }
      bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+31)/
        32);
      break;
    }
    case 32:
    {
      bytes_per_row=4*(((size_t) samples_per_pixel*columns*bits_per_pixel+31)/
        32);
      break;
    }
    case 64:
    {
      bytes_per_row=8*(((size_t) samples_per_pixel*columns*bits_per_pixel+63)/
        64);
      break;
    }
  }
  return(bytes_per_row);
}