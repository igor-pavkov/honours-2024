static int PNMComment(Image *image,ExceptionInfo *exception)
{
  int
    c;

  char
    *comment;

  register char
    *p;

  size_t
    extent;

  /*
    Read comment.
  */
  comment=AcquireString(GetImageProperty(image,"comment",exception));
  p=comment+strlen(comment);
  extent=strlen(comment)+MagickPathExtent;
  for (c='#'; (c != EOF) && (c != (int) '\n'); p++)
  {
    if ((size_t) (p-comment+1) >= extent)
      {
        extent<<=1;
        comment=(char *) ResizeQuantumMemory(comment,extent+MagickPathExtent,
          sizeof(*comment));
        if (comment == (char *) NULL)
          break;
        p=comment+strlen(comment);
      }
    c=ReadBlobByte(image);
    if (c != EOF)
      {
        *p=(char) c;
        *(p+1)='\0';
      }
  }
  if (comment == (char *) NULL)
    return(c);
  (void) SetImageProperty(image,"comment",comment,exception);
  comment=DestroyString(comment);
  return(c);
}