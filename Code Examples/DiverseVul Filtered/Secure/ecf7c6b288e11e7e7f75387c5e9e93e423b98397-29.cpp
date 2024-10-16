static MagickBooleanType TraceRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end)
{
  PointInfo
    point;

  register PrimitiveInfo
    *p;

  register ssize_t
    i;

  p=primitive_info;
  if (TracePoint(p,start) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  if (TracePoint(p,point) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  if (TracePoint(p,end) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  if (TracePoint(p,point) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  if (TracePoint(p,start) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  primitive_info->closed_subpath=MagickTrue;
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  return(MagickTrue);
}