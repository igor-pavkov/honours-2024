MagickExport PixelInfo *GetImageHistogram(const Image *image,
  size_t *number_colors,ExceptionInfo *exception)
{
  PixelInfo
    *histogram;

  CubeInfo
    *cube_info;

  *number_colors=0;
  histogram=(PixelInfo *) NULL;
  cube_info=ClassifyImageColors(image,exception);
  if (cube_info != (CubeInfo *) NULL)
    {
      histogram=(PixelInfo *) AcquireQuantumMemory((size_t) cube_info->colors+1,
        sizeof(*histogram));
      if (histogram == (PixelInfo *) NULL)
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      else
        {
          PixelInfo
            *root;

          *number_colors=cube_info->colors;
          root=histogram;
          DefineImageHistogram(image,cube_info->root,&root);
        }
    }
  cube_info=DestroyCubeInfo(image,cube_info);
  return(histogram);
}