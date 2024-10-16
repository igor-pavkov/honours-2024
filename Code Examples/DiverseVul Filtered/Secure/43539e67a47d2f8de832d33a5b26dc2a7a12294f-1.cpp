static void Hq2XHelper(const unsigned int rule,const Quantum *source,
  Quantum *destination,const ssize_t destination_offset,const size_t channels,
  const ssize_t e,const ssize_t a,const ssize_t b,const ssize_t d,
  const ssize_t f,const ssize_t h)
{
#define caseA(N,A,B,C,D) \
  case N: \
  { \
    const ssize_t \
      offsets[4] = { A, B, C, D }; \
 \
    MixPixels(source,offsets,4,destination,destination_offset,channels);\
    break; \
  }
#define caseB(N,A,B,C,D,E,F,G,H) \
  case N: \
  { \
    const ssize_t \
      offsets[8] = { A, B, C, D, E, F, G, H }; \
 \
    MixPixels(source,offsets,8,destination,destination_offset,channels);\
    break; \
  }

  switch (rule)
  {
    case 0:
    {
      CopyPixels(source,e,destination,destination_offset,channels);
      break;
    }
    caseA(1,e,e,e,a)
    caseA(2,e,e,e,d)
    caseA(3,e,e,e,b)
    caseA(4,e,e,d,b)
    caseA(5,e,e,a,b)
    caseA(6,e,e,a,d)
    caseB(7,e,e,e,e,e,b,b,d)
    caseB(8,e,e,e,e,e,d,d,b)
    caseB(9,e,e,e,e,e,e,d,b)
    caseB(10,e,e,d,d,d,b,b,b)
    case 11:
    {
      const ssize_t
        offsets[16] = { e, e, e, e, e, e, e, e, e, e, e, e, e, e, d, b };

      MixPixels(source,offsets,16,destination,destination_offset,channels);
      break;
    }
    case 12:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[4] = { e, e, d, b };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      else
        CopyPixels(source,e,destination,destination_offset,channels);
      break;
    }
    case 13:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[8] = { e, e, d, d, d, b, b, b };

          MixPixels(source,offsets,8,destination,destination_offset,channels);
        }
      else
        CopyPixels(source,e,destination,destination_offset,channels);
      break;
    }
    case 14:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[16] = { e, e, e, e, e, e, e, e, e, e, e, e, e, e, d, b };

          MixPixels(source,offsets,16,destination,destination_offset,channels);
        }
      else
        CopyPixels(source,e,destination,destination_offset,channels);
      break;
    }
    case 15:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[4] = { e, e, d, b };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      else
        {
          const ssize_t
            offsets[4] = { e, e, e, a };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      break;
    }
    case 16:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[8] = { e, e, e, e, e, e, d, b };

          MixPixels(source,offsets,8,destination,destination_offset,channels);
        }
      else
        {
          const ssize_t
            offsets[4] = { e, e, e, a };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      break;
    }
    case 17:
    {
      if (PixelsEqual(source,b,source,d,channels))
        {
          const ssize_t
            offsets[8] = { e, e, d, d, d, b, b, b };

          MixPixels(source,offsets,8,destination,destination_offset,channels);
        }
      else
        {
          const ssize_t
            offsets[4] = { e, e, e, a };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      break;
    }
    case 18:
    {
      if (PixelsEqual(source,b,source,f,channels))
        {
          const ssize_t
            offsets[8] = { e, e, e, e, e, b, b, d };

          MixPixels(source,offsets,8,destination,destination_offset,channels);
        }
      else
        {
          const ssize_t
            offsets[4] = { e, e, e, d };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      break;
    }
    default:
    {
      if (PixelsEqual(source,d,source,h,channels))
        {
          const ssize_t
            offsets[8] = { e, e, e, e, e, d, d, b };

          MixPixels(source,offsets,8,destination,destination_offset,channels);
        }
      else
        {
          const ssize_t
            offsets[4] = { e, e, e, b };

          MixPixels(source,offsets,4,destination,destination_offset,channels);
        }
      break;
    }
  }
  #undef caseA
  #undef caseB
}