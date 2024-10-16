static inline MagickBooleanType TracePoint(PrimitiveInfo *primitive_info,
  const PointInfo point)
{
  primitive_info->coordinates=1;
  primitive_info->closed_subpath=MagickFalse;
  primitive_info->point=point;
  return(MagickTrue);
}