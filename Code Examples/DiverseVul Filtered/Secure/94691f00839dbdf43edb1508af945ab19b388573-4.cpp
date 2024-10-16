static MagickBooleanType CheckImageColors(const Image *image,
  ExceptionInfo *exception,size_t max_colors)
{
  CacheView
    *image_view;

  CubeInfo
    *cube_info;

  PixelInfo
    pixel,
    target;

  register const Quantum
    *p;

  register ssize_t
    x;

  register NodeInfo
    *node_info;

  register ssize_t
    i;

  size_t
    id,
    index,
    level;

  ssize_t
    y;

  if (image->storage_class == PseudoClass)
    return((image->colors <= max_colors) ? MagickTrue : MagickFalse);
  /*
    Initialize color description tree.
  */
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(MagickFalse);
    }
  GetPixelInfo(image,&pixel);
  GetPixelInfo(image,&target);
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
                break;
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      if (level < MaxTreeDepth)
        break;
      for (i=0; i < (ssize_t) node_info->number_unique; i++)
      {
        target=node_info->list[i];
        if (IsPixelInfoEquivalent(&pixel,&target) != MagickFalse)
          break;
      }
      if (i < (ssize_t) node_info->number_unique)
        node_info->list[i].count++;
      else
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=(PixelInfo *) AcquireMagickMemory(
              sizeof(*node_info->list));
          else
            node_info->list=(PixelInfo *) ResizeQuantumMemory(node_info->list,
              (size_t) (i+1),sizeof(*node_info->list));
          if (node_info->list == (PixelInfo *) NULL)
            {
              (void) ThrowMagickException(exception,GetMagickModule(),
                ResourceLimitError,"MemoryAllocationFailed","`%s'",
                image->filename);
              break;
            }
          GetPixelInfo(image,&node_info->list[i]);
          node_info->list[i].red=(double) GetPixelRed(image,p);
          node_info->list[i].green=(double) GetPixelGreen(image,p);
          node_info->list[i].blue=(double) GetPixelBlue(image,p);
          if (image->colorspace == CMYKColorspace)
            node_info->list[i].black=(double) GetPixelBlack(image,p);
          node_info->list[i].alpha=(double) GetPixelAlpha(image,p);
          node_info->list[i].count=1;
          node_info->number_unique++;
          cube_info->colors++;
          if (cube_info->colors > max_colors)
            break;
        }
      p+=GetPixelChannels(image);
    }
    if (x < (ssize_t) image->columns)
      break;
  }
  image_view=DestroyCacheView(image_view);
  cube_info=DestroyCubeInfo(image,cube_info);
  return(y < (ssize_t) image->rows ? MagickFalse : MagickTrue);
}