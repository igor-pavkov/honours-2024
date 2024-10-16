MagickExport double InterpretSiPrefixValue(const char *magick_restrict string,
  char **magick_restrict sentinal)
{
  char
    *q;

  double
    value;

  value=InterpretLocaleValue(string,&q);
  if (q != string)
    {
      if ((*q >= 'E') && (*q <= 'z'))
        {
          double
            e;

          switch ((int) ((unsigned char) *q))
          {
            case 'y': e=(-24.0); break;
            case 'z': e=(-21.0); break;
            case 'a': e=(-18.0); break;
            case 'f': e=(-15.0); break;
            case 'p': e=(-12.0); break;
            case 'n': e=(-9.0); break;
            case 'u': e=(-6.0); break;
            case 'm': e=(-3.0); break;
            case 'c': e=(-2.0); break;
            case 'd': e=(-1.0); break;
            case 'h': e=2.0; break;
            case 'k': e=3.0; break;
            case 'K': e=3.0; break;
            case 'M': e=6.0; break;
            case 'G': e=9.0; break;
            case 'T': e=12.0; break;
            case 'P': e=15.0; break;
            case 'E': e=18.0; break;
            case 'Z': e=21.0; break;
            case 'Y': e=24.0; break;
            default: e=0.0; break;
          }
          if (e >= MagickEpsilon)
            {
              if (q[1] == 'i')
                {
                  value*=pow(2.0,e/0.3);
                  q+=2;
                }
              else
                {
                  value*=pow(10.0,e);
                  q++;
                }
            }
        }
      if ((*q == 'B') || (*q == 'P'))
        q++;
    }
  if (sentinal != (char **) NULL)
    *sentinal=q;
  return(value);
}