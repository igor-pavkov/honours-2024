MagickExport Image *ResizeImage(const Image *image,const size_t columns,
  const size_t rows,const FilterType filter,ExceptionInfo *exception)
{
  double
    x_factor,
    y_factor;

  FilterType
    filter_type;

  Image
    *filter_image,
    *resize_image;

  MagickOffsetType
    offset;

  MagickSizeType
    span;

  MagickStatusType
    status;

  ResizeFilter
    *resize_filter;

  /*
    Acquire resize image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(ImageError,"NegativeOrZeroImageSize");
  if ((columns == image->columns) && (rows == image->rows) &&
      (filter == UndefinedFilter))
    return(CloneImage(image,0,0,MagickTrue,exception));
  /*
    Acquire resize filter.
  */
  x_factor=(double) columns/(double) image->columns;
  y_factor=(double) rows/(double) image->rows;
  filter_type=LanczosFilter;
  if (filter != UndefinedFilter)
    filter_type=filter;
  else
    if ((x_factor == 1.0) && (y_factor == 1.0))
      filter_type=PointFilter;
    else
      if ((image->storage_class == PseudoClass) ||
          (image->alpha_trait != UndefinedPixelTrait) ||
          ((x_factor*y_factor) > 1.0))
        filter_type=MitchellFilter;
  resize_filter=AcquireResizeFilter(image,filter_type,MagickFalse,exception);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  resize_image=AccelerateResizeImage(image,columns,rows,resize_filter,
    exception);
  if (resize_image != (Image *) NULL)
    {
      resize_filter=DestroyResizeFilter(resize_filter);
      return(resize_image);
    }
#endif
  resize_image=CloneImage(image,columns,rows,MagickTrue,exception);
  if (resize_image == (Image *) NULL)
    {
      resize_filter=DestroyResizeFilter(resize_filter);
      return(resize_image);
    }
  if (x_factor > y_factor)
    filter_image=CloneImage(image,columns,image->rows,MagickTrue,exception);
  else
    filter_image=CloneImage(image,image->columns,rows,MagickTrue,exception);
  if (filter_image == (Image *) NULL)
    {
      resize_filter=DestroyResizeFilter(resize_filter);
      return(DestroyImage(resize_image));
    }
  /*
    Resize image.
  */
  offset=0;
  if (x_factor > y_factor)
    {
      span=(MagickSizeType) (filter_image->columns+rows);
      status=HorizontalFilter(resize_filter,image,filter_image,x_factor,span,
        &offset,exception);
      status&=VerticalFilter(resize_filter,filter_image,resize_image,y_factor,
        span,&offset,exception);
    }
  else
    {
      span=(MagickSizeType) (filter_image->rows+columns);
      status=VerticalFilter(resize_filter,image,filter_image,y_factor,span,
        &offset,exception);
      status&=HorizontalFilter(resize_filter,filter_image,resize_image,x_factor,
        span,&offset,exception);
    }
  /*
    Free resources.
  */
  filter_image=DestroyImage(filter_image);
  resize_filter=DestroyResizeFilter(resize_filter);
  if (status == MagickFalse)
    {
      resize_image=DestroyImage(resize_image);
      return((Image *) NULL);
    }
  resize_image->type=image->type;
  return(resize_image);
}