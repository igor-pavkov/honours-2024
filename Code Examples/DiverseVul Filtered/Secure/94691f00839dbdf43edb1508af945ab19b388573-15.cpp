static void DestroyColorCube(const Image *image,NodeInfo *node_info)
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
      DestroyColorCube(image,node_info->child[i]);
  if (node_info->list != (PixelInfo *) NULL)
    node_info->list=(PixelInfo *) RelinquishMagickMemory(node_info->list);
}