static void UniqueColorsToImage(Image *unique_image,CacheView *unique_view,
  CubeInfo *cube_info,const NodeInfo *node_info,ExceptionInfo *exception)
{
#define UniqueColorsImageTag  "UniqueColors/Image"

  MagickBooleanType
    status;

  register ssize_t
    i;

  size_t
    number_children;

  /*
    Traverse any children.
  */
  number_children=unique_image->alpha_trait == UndefinedPixelTrait ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      UniqueColorsToImage(unique_image,unique_view,cube_info,
        node_info->child[i],exception);
  if (node_info->level == (MaxTreeDepth-1))
    {
      register PixelInfo
        *p;

      register Quantum
        *magick_restrict q;

      status=MagickTrue;
      p=node_info->list;
      for (i=0; i < (ssize_t) node_info->number_unique; i++)
      {
        q=QueueCacheViewAuthenticPixels(unique_view,cube_info->x,0,1,1,
          exception);
        if (q == (Quantum *) NULL)
          continue;
        SetPixelRed(unique_image,ClampToQuantum(p->red),q);
        SetPixelGreen(unique_image,ClampToQuantum(p->green),q);
        SetPixelBlue(unique_image,ClampToQuantum(p->blue),q);
        SetPixelAlpha(unique_image,ClampToQuantum(p->alpha),q);
        if (unique_image->colorspace == CMYKColorspace)
          SetPixelBlack(unique_image,ClampToQuantum(p->black),q);
        if (SyncCacheViewAuthenticPixels(unique_view,exception) == MagickFalse)
          break;
        cube_info->x++;
        p++;
      }
      if (unique_image->progress_monitor != (MagickProgressMonitor) NULL)
        {
          MagickBooleanType
            proceed;

          proceed=SetImageProgress(unique_image,UniqueColorsImageTag,
            cube_info->progress,cube_info->colors);
          if (proceed == MagickFalse)
            status=MagickFalse;
        }
      cube_info->progress++;
      if (status == MagickFalse)
        return;
    }
}