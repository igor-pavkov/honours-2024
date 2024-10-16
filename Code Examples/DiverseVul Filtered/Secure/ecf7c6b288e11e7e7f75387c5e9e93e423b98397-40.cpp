MagickExport MagickBooleanType DrawImage(Image *image,const DrawInfo *draw_info,
  ExceptionInfo *exception)
{
  return(RenderMVGContent(image,draw_info,0,exception));
}