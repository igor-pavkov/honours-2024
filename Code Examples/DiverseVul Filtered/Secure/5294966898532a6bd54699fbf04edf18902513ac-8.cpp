static size_t DefineImageColormap(Image *image,CubeInfo *cube_info,
  NodeInfo *node_info)
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
      (void) DefineImageColormap(image,cube_info,node_info->child[i]);
  if (node_info->number_unique != 0)
    {
      register double
        alpha;

      register PixelInfo
        *magick_restrict q;

      /*
        Colormap entry is defined by the mean color in this cube.
      */
      q=image->colormap+image->colors;
      alpha=(double) ((MagickOffsetType) node_info->number_unique);
      alpha=PerceptibleReciprocal(alpha);
      if (cube_info->associate_alpha == MagickFalse)
        {
          q->red=(double) ClampToQuantum(alpha*QuantumRange*
            node_info->total_color.red);
          q->green=(double) ClampToQuantum(alpha*QuantumRange*
            node_info->total_color.green);
          q->blue=(double) ClampToQuantum(alpha*QuantumRange*
            node_info->total_color.blue);
          q->alpha=(double) OpaqueAlpha;
        }
      else
        {
          double
            opacity;

          opacity=(double) (alpha*QuantumRange*node_info->total_color.alpha);
          q->alpha=(double) ClampToQuantum(opacity);
          if (q->alpha == OpaqueAlpha)
            {
              q->red=(double) ClampToQuantum(alpha*QuantumRange*
                node_info->total_color.red);
              q->green=(double) ClampToQuantum(alpha*QuantumRange*
                node_info->total_color.green);
              q->blue=(double) ClampToQuantum(alpha*QuantumRange*
                node_info->total_color.blue);
            }
          else
            {
              double
                gamma;

              gamma=(double) (QuantumScale*q->alpha);
              gamma=PerceptibleReciprocal(gamma);
              q->red=(double) ClampToQuantum(alpha*gamma*QuantumRange*
                node_info->total_color.red);
              q->green=(double) ClampToQuantum(alpha*gamma*QuantumRange*
                node_info->total_color.green);
              q->blue=(double) ClampToQuantum(alpha*gamma*QuantumRange*
                node_info->total_color.blue);
              if (node_info->number_unique > cube_info->transparent_pixels)
                {
                  cube_info->transparent_pixels=node_info->number_unique;
                  cube_info->transparent_index=(ssize_t) image->colors;
                }
            }
        }
      node_info->color_number=image->colors++;
    }
  return(image->colors);
}