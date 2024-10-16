static CubeInfo *ClassifyImageColors(const Image *image,
  ExceptionInfo *exception)
{
#define EvaluateImageTag  "  Compute image colors...  "

  CacheView
    *image_view;

  CubeInfo
    *cube_info;

  MagickBooleanType
    proceed;

  PixelInfo
    pixel;

  NodeInfo
    *node_info;

  register const Quantum
    *p;

  register size_t
    id,
    index,
    level;

  register ssize_t
    i,
    x;

  ssize_t
    y;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(cube_info);
    }
  GetPixelInfo(image,&pixel);
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        GetPixelInfoPixel(image,p,&pixel);
        id=ColorToNodeId(image,&pixel,index);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","`%s'",
                  image->filename);
                return(0);
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (ssize_t) node_info->number_unique; i++)
        if (IsPixelInfoEquivalent(&pixel,node_info->list+i) != MagickFalse)
          break;
      if (i < (ssize_t) node_info->number_unique)
        node_info->list[i].count++;
      else
        {
          if (node_info->number_unique == 0)
            {
              node_info->extent=1;
              node_info->list=(PixelInfo *) AcquireQuantumMemory(
                node_info->extent,sizeof(*node_info->list));
            }
          else
            if (i >= (ssize_t) node_info->extent)
              {
                node_info->extent<<=1;
                node_info->list=(PixelInfo *) ResizeQuantumMemory(
                  node_info->list,node_info->extent,sizeof(*node_info->list));
              }
          if (node_info->list == (PixelInfo *) NULL)
            {
              (void) ThrowMagickException(exception,GetMagickModule(),
                ResourceLimitError,"MemoryAllocationFailed","`%s'",
                image->filename);
              return(0);
            }
          node_info->list[i]=pixel;
          node_info->list[i].red=(double) GetPixelRed(image,p);
          node_info->list[i].green=(double) GetPixelGreen(image,p);
          node_info->list[i].blue=(double) GetPixelBlue(image,p);
          if (image->colorspace == CMYKColorspace)
            node_info->list[i].black=(double) GetPixelBlack(image,p);
          node_info->list[i].alpha=(double) GetPixelAlpha(image,p);
          node_info->list[i].count=1;
          node_info->number_unique++;
          cube_info->colors++;
        }
      p+=GetPixelChannels(image);
    }
    proceed=SetImageProgress(image,EvaluateImageTag,(MagickOffsetType) y,
      image->rows);
    if (proceed == MagickFalse)
      break;
  }
  image_view=DestroyCacheView(image_view);
  return(cube_info);
}