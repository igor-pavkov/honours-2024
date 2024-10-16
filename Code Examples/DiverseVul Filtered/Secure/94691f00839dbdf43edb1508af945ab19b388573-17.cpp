static CubeInfo *DestroyCubeInfo(const Image *image,CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  DestroyColorCube(image,cube_info->root);
  do
  {
    nodes=cube_info->node_queue->next;
    cube_info->node_queue=(Nodes *)
      RelinquishMagickMemory(cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  return((CubeInfo *) RelinquishMagickMemory(cube_info));
}