BGD_DECLARE(gdImagePtr) gdImageCreateFromXbm (FILE * fd)
{
  gdImagePtr im;
  int bit;
  int w, h;
  int bytes;
  int ch;
  int i, x, y;
  char *sp;
  char s[161];
  if (!fgets (s, 160, fd))
    {
      return 0;
    }
  sp = &s[0];
  /* Skip #define */
  sp = strchr (sp, ' ');
  if (!sp)
    {
      return 0;
    }
  /* Skip width label */
  sp++;
  sp = strchr (sp, ' ');
  if (!sp)
    {
      return 0;
    }
  /* Get width */
  w = atoi (sp + 1);
  if (!w)
    {
      return 0;
    }
  if (!fgets (s, 160, fd))
    {
      return 0;
    }
  sp = s;
  /* Skip #define */
  sp = strchr (sp, ' ');
  if (!sp)
    {
      return 0;
    }
  /* Skip height label */
  sp++;
  sp = strchr (sp, ' ');
  if (!sp)
    {
      return 0;
    }
  /* Get height */
  h = atoi (sp + 1);
  if (!h)
    {
      return 0;
    }
  /* Skip declaration line */
  if (!fgets (s, 160, fd))
    {
      return 0;
    }
  bytes = (w * h / 8) + 1;
  im = gdImageCreate (w, h);
  gdImageColorAllocate (im, 255, 255, 255);
  gdImageColorAllocate (im, 0, 0, 0);
  x = 0;
  y = 0;
  for (i = 0; (i < bytes); i++)
    {
      char h[3];
      unsigned int b;
      /* Skip spaces, commas, CRs, 0x */
      while (1)
	{
	  ch = getc (fd);
	  if (ch == EOF)
	    {
	      goto fail;
	    }
	  if (ch == 'x')
	    {
	      break;
	    }
	}
      /* Get hex value */
      ch = getc (fd);
      if (ch == EOF)
	{
	  goto fail;
	}
      h[0] = ch;
      ch = getc (fd);
      if (ch == EOF)
	{
	  goto fail;
	}
      h[1] = ch;
      h[2] = '\0';
      sscanf (h, "%x", &b);
      for (bit = 1; (bit <= 128); (bit = bit << 1))
	{
	  gdImageSetPixel (im, x++, y, (b & bit) ? 1 : 0);
	  if (x == im->sx)
	    {
	      x = 0;
	      y++;
	      if (y == im->sy)
		{
		  return im;
		}
	      /* Fix 8/8/95 */
	      break;
	    }
	}
    }
  /* Shouldn't happen */
  fprintf (stderr, "Error: bug in gdImageCreateFromXbm!\n");
  return 0;
fail:
  gdImageDestroy (im);
  return 0;
}