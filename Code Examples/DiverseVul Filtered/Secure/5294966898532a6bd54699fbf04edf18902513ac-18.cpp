static void ClosestColor(const Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info)
{
  register ssize_t
    i;

  size_t
    number_children;

  /*
    Traverse any children.
  */
  number_children=cube_info->associate_alpha == MagickFalse ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      ClosestColor(image,cube_info,node_info->child[i]);
  if (node_info->number_unique != 0)
    {
      double
        pixel;

      register double
        alpha,
        beta,
        distance;

      register DoublePixelPacket
        *magick_restrict q;

      register PixelInfo
        *magick_restrict p;

      /*
        Determine if this color is "closest".
      */
      p=image->colormap+node_info->color_number;
      q=(&cube_info->target);
      alpha=1.0;
      beta=1.0;
      if (cube_info->associate_alpha != MagickFalse)
        {
          alpha=(double) (QuantumScale*p->alpha);
          beta=(double) (QuantumScale*q->alpha);
        }
      pixel=alpha*p->red-beta*q->red;
      distance=pixel*pixel;
      if (distance <= cube_info->distance)
        {
          pixel=alpha*p->green-beta*q->green;
          distance+=pixel*pixel;
          if (distance <= cube_info->distance)
            {
              pixel=alpha*p->blue-beta*q->blue;
              distance+=pixel*pixel;
              if (distance <= cube_info->distance)
                {
                  if (cube_info->associate_alpha != MagickFalse)
                    {
                      pixel=p->alpha-q->alpha;
                      distance+=pixel*pixel;
                    }
                  if (distance <= cube_info->distance)
                    {
                      cube_info->distance=distance;
                      cube_info->color_number=node_info->color_number;
                    }
                }
            }
        }
    }
}