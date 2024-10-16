static void DefineImageHistogram(const Image *image,NodeInfo *node_info,
  PixelInfo **histogram)
{
  register ssize_t
    i;

  size_t
    number_children;

  /*
    Traverse any children.
  */
  number_children=image->alpha_trait == UndefinedPixelTrait ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      DefineImageHistogram(image,node_info->child[i],histogram);
  if (node_info->level == (MaxTreeDepth-1))
    {
      register PixelInfo
        *p;

      p=node_info->list;
      for (i=0; i < (ssize_t) node_info->number_unique; i++)
      {
        **histogram=(*p);
        (*histogram)++;
        p++;
      }
    }
}