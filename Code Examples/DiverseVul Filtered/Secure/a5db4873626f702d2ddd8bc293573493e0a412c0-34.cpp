static void AffineToTransform(Image *image,AffineMatrix *affine)
{
  char
    transform[MaxTextExtent];

  if ((fabs(affine->tx) < MagickEpsilon) && (fabs(affine->ty) < MagickEpsilon))
    {
      if ((fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon))
        {
          if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
              (fabs(affine->sy-1.0) < MagickEpsilon))
            {
              (void) WriteBlobString(image,"\">\n");
              return;
            }
          (void) FormatLocaleString(transform,MaxTextExtent,
            "\" transform=\"scale(%g,%g)\">\n",affine->sx,affine->sy);
          (void) WriteBlobString(image,transform);
          return;
        }
      else
        {
          if ((fabs(affine->sx-affine->sy) < MagickEpsilon) &&
              (fabs(affine->rx+affine->ry) < MagickEpsilon) &&
              (fabs(affine->sx*affine->sx+affine->rx*affine->rx-1.0) <
               2*MagickEpsilon))
            {
              double
                theta;

              theta=(180.0/MagickPI)*atan2(affine->rx,affine->sx);
              (void) FormatLocaleString(transform,MaxTextExtent,
                "\" transform=\"rotate(%g)\">\n",theta);
              (void) WriteBlobString(image,transform);
              return;
            }
        }
    }
  else
    {
      if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
          (fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon) &&
          (fabs(affine->sy-1.0) < MagickEpsilon))
        {
          (void) FormatLocaleString(transform,MaxTextExtent,
            "\" transform=\"translate(%g,%g)\">\n",affine->tx,affine->ty);
          (void) WriteBlobString(image,transform);
          return;
        }
    }
  (void) FormatLocaleString(transform,MaxTextExtent,
    "\" transform=\"matrix(%g %g %g %g %g %g)\">\n",
    affine->sx,affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
  (void) WriteBlobString(image,transform);
}