static RectangleInfo CompareImageBounds(const Image *image1,const Image *image2,
  const ImageLayerMethod method,ExceptionInfo *exception)
{
  RectangleInfo
    bounds;

  MagickPixelPacket
    pixel1,
    pixel2;

  register const IndexPacket
    *indexes1,
    *indexes2;

  register const PixelPacket
    *p,
    *q;

  register ssize_t
    x;

  ssize_t
    y;

#if 0
  /* only same sized images can be compared */
  assert(image1->columns == image2->columns);
  assert(image1->rows == image2->rows);
#endif

  /*
    Set bounding box of the differences between images
  */
  GetMagickPixelPacket(image1,&pixel1);
  GetMagickPixelPacket(image2,&pixel2);
  for (x=0; x < (ssize_t) image1->columns; x++)
  {
    p=GetVirtualPixels(image1,x,0,1,image1->rows,exception);
    q=GetVirtualPixels(image2,x,0,1,image2->rows,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (const PixelPacket *) NULL))
      break;
    indexes1=GetVirtualIndexQueue(image1);
    indexes2=GetVirtualIndexQueue(image2);
    for (y=0; y < (ssize_t) image1->rows; y++)
    {
      SetMagickPixelPacket(image1,p,indexes1+x,&pixel1);
      SetMagickPixelPacket(image2,q,indexes2+x,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p++;
      q++;
    }
    if (y < (ssize_t) image1->rows)
      break;
  }
  if (x >= (ssize_t) image1->columns)
    {
      /*
        Images are identical, return a null image.
      */
      bounds.x=-1;
      bounds.y=-1;
      bounds.width=1;
      bounds.height=1;
      return(bounds);
    }
  bounds.x=x;
  for (x=(ssize_t) image1->columns-1; x >= 0; x--)
  {
    p=GetVirtualPixels(image1,x,0,1,image1->rows,exception);
    q=GetVirtualPixels(image2,x,0,1,image2->rows,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (const PixelPacket *) NULL))
      break;
    indexes1=GetVirtualIndexQueue(image1);
    indexes2=GetVirtualIndexQueue(image2);
    for (y=0; y < (ssize_t) image1->rows; y++)
    {
      SetMagickPixelPacket(image1,p,indexes1+x,&pixel1);
      SetMagickPixelPacket(image2,q,indexes2+x,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p++;
      q++;
    }
    if (y < (ssize_t) image1->rows)
      break;
  }
  bounds.width=(size_t) (x-bounds.x+1);
  for (y=0; y < (ssize_t) image1->rows; y++)
  {
    p=GetVirtualPixels(image1,0,y,image1->columns,1,exception);
    q=GetVirtualPixels(image2,0,y,image2->columns,1,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (const PixelPacket *) NULL))
      break;
    indexes1=GetVirtualIndexQueue(image1);
    indexes2=GetVirtualIndexQueue(image2);
    for (x=0; x < (ssize_t) image1->columns; x++)
    {
      SetMagickPixelPacket(image1,p,indexes1+x,&pixel1);
      SetMagickPixelPacket(image2,q,indexes2+x,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p++;
      q++;
    }
    if (x < (ssize_t) image1->columns)
      break;
  }
  bounds.y=y;
  for (y=(ssize_t) image1->rows-1; y >= 0; y--)
  {
    p=GetVirtualPixels(image1,0,y,image1->columns,1,exception);
    q=GetVirtualPixels(image2,0,y,image2->columns,1,exception);
    if ((p == (const PixelPacket *) NULL) || (q == (const PixelPacket *) NULL))
      break;
    indexes1=GetVirtualIndexQueue(image1);
    indexes2=GetVirtualIndexQueue(image2);
    for (x=0; x < (ssize_t) image1->columns; x++)
    {
      SetMagickPixelPacket(image1,p,indexes1+x,&pixel1);
      SetMagickPixelPacket(image2,q,indexes2+x,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p++;
      q++;
    }
    if (x < (ssize_t) image1->columns)
      break;
  }
  bounds.height=(size_t) (y-bounds.y+1);
  return(bounds);
}