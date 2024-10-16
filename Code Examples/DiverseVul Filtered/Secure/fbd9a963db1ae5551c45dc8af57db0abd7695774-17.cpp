MagickExport double ExpandAffine(const AffineMatrix *affine)
{
  assert(affine != (const AffineMatrix *) NULL);
  return(sqrt(fabs(affine->sx*affine->sy-affine->rx*affine->ry)));
}