static MagickBooleanType TraceLine(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end)
{
  if (TracePoint(primitive_info,start) == MagickFalse)
    return(MagickFalse);
  if ((fabs(start.x-end.x) < MagickEpsilon) &&
      (fabs(start.y-end.y) < MagickEpsilon))
    {
      primitive_info->primitive=PointPrimitive;
      primitive_info->coordinates=1;
      return(MagickTrue);
    }
  if (TracePoint(primitive_info+1,end) == MagickFalse)
    return(MagickFalse);
  (primitive_info+1)->primitive=primitive_info->primitive;
  primitive_info->coordinates=2;
  primitive_info->closed_subpath=MagickFalse;
  return(MagickTrue);
}