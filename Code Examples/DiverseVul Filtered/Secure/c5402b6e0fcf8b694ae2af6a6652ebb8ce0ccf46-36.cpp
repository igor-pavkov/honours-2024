MagickExport Image *AdaptiveResizeImage(const Image *image,
  const size_t columns,const size_t rows,ExceptionInfo *exception)
{
  Image
    *resize_image;

  resize_image=InterpolativeResizeImage(image,columns,rows,MeshInterpolatePixel,
    exception);
  return(resize_image);
}