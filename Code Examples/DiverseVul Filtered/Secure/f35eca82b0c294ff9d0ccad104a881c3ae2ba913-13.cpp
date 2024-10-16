MagickExport MagickStatusType GetGeometry(const char *geometry,ssize_t *x,
  ssize_t *y,size_t *width,size_t *height)
{
  char
    *p,
    pedantic_geometry[MagickPathExtent],
    *q;

  double
    value;

  int
    c;

  MagickStatusType
    flags;

  /*
    Remove whitespace and meta characters from geometry specification.
  */
  flags=NoValue;
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(flags);
  if (strlen(geometry) >= (MagickPathExtent-1))
    return(flags);
  (void) CopyMagickString(pedantic_geometry,geometry,MagickPathExtent);
  for (p=pedantic_geometry; *p != '\0'; )
  {
    if (isspace((int) ((unsigned char) *p)) != 0)
      {
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        continue;
      }
    c=(int)*p;
    switch (c)
    {
      case '%':
      {
        flags|=PercentValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '!':
      {
        flags|=AspectValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '<':
      {
        flags|=LessValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '>':
      {
        flags|=GreaterValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '^':
      {
        flags|=MinimumValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '@':
      {
        flags|=AreaValue;
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case '(':
      case ')':
      {
        (void) CopyMagickString(p,p+1,MagickPathExtent);
        break;
      }
      case 'x':
      case 'X':
      {
        flags|=SeparatorValue;
        p++;
        break;
      }
      case '-':
      case ',':
      case '+':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 215:
      case 'e':
      case 'E':
      {
        p++;
        break;
      }
      case '.':
      {
        p++;
        flags|=DecimalValue;
        break;
      }
      case ':':
      {
        p++;
        flags|=AspectRatioValue;
        break;
      }
      default:
        return(flags);
    }
  }
  /*
    Parse width, height, x, and y.
  */
  p=pedantic_geometry;
  if (*p == '\0')
    return(flags);
  q=p;
  value=StringToDouble(p,&q);
  (void) value;
  if (LocaleNCompare(p,"0x",2) == 0)
    value=(double) strtol(p,&q,10);
  if ((*p != '+') && (*p != '-'))
    {
      c=(int) ((unsigned char) *q);
      if ((c == 215) || (*q == 'x') || (*q == 'X') || (*q == ':') ||
          (*q == '\0'))
        {
          /*
            Parse width.
          */
          q=p;
          if (width != (size_t *) NULL)
            {
              if (LocaleNCompare(p,"0x",2) == 0)
                *width=(size_t) strtol(p,&p,10);
              else
                *width=((size_t) floor(StringToDouble(p,&p)+0.5)) & 0x7fffffff;
            }
          if (p != q)
            flags|=WidthValue;
        }
    }
  if ((*p != '+') && (*p != '-'))
    {
      c=(int) ((unsigned char) *p);
      if ((c == 215) || (*p == 'x') || (*p == 'X') || (*p == ':'))
        {
          p++;
          if ((*p != '+') && (*p != '-'))
            {
              /*
                Parse height.
              */
              q=p;
              if (height != (size_t *) NULL)
                *height=((size_t) floor(StringToDouble(p,&p)+0.5)) & 0x7fffffff;
              if (p != q)
                flags|=HeightValue;
            }
        }
    }
  if ((*p == '+') || (*p == '-'))
    {
      /*
        Parse x value.
      */
      while ((*p == '+') || (*p == '-'))
      {
        if (*p == '-')
          flags^=XNegative;  /* negate sign */
        p++;
      }
      q=p;
      if (x != (ssize_t *) NULL)
        *x=((ssize_t) ceil(StringToDouble(p,&p)-0.5)) & 0x7fffffff;
      if (p != q)
        {
          flags|=XValue;
          if (((flags & XNegative) != 0) && (x != (ssize_t *) NULL))
            *x=(-*x);
        }
    }
  if ((*p == '+') || (*p == '-'))
    {
      /*
        Parse y value.
      */
      while ((*p == '+') || (*p == '-'))
      {
        if (*p == '-')
          flags^=YNegative;  /* negate sign */
        p++;
      }
      q=p;
      if (y != (ssize_t *) NULL)
        *y=((ssize_t) ceil(StringToDouble(p,&p)-0.5)) & 0x7fffffff;
      if (p != q)
        {
          flags|=YValue;
          if (((flags & YNegative) != 0) && (y != (ssize_t *) NULL))
            *y=(-*y);
        }
    }
  if ((flags & PercentValue) != 0)
    {
      if (((flags & SeparatorValue) == 0) && ((flags & HeightValue) == 0))
        {
          if ((height != (size_t *) NULL) && (width != (size_t *) NULL))
            *height=(*width);
          flags|=HeightValue;
        }
      if (((flags & SeparatorValue) != 0) && ((flags & WidthValue) == 0) &&
          (height != (size_t *) NULL) && (width != (size_t *) NULL))
        *width=(*height);
    }
#if 0
  /* Debugging Geometry */
  (void) fprintf(stderr,"GetGeometry...\n");
  (void) fprintf(stderr,"Input: %s\n",geometry);
  (void) fprintf(stderr,"Flags: %c %c %s %s\n",
    (flags & WidthValue) ? 'W' : ' ',(flags & HeightValue) ? 'H' : ' ',
    (flags & XValue) ? ((flags & XNegative) ? "-X" : "+X") : "  ",
    (flags & YValue) ? ((flags & YNegative) ? "-Y" : "+Y") : "  ");
  (void) fprintf(stderr,"Geometry: %ldx%ld%+ld%+ld\n",(long) *width,(long)
    *height,(long) *x,(long) *y);
#endif
  return(flags);
}