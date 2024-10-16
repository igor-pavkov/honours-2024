static MagickBooleanType TraceEllipse(MVGInfo *mvg_info,const PointInfo center,
  const PointInfo radii,const PointInfo arc)
{
  double
    coordinates,
    delta,
    step,
    x,
    y;

  PointInfo
    angle,
    point;

  PrimitiveInfo
    *primitive_info;

  register PrimitiveInfo
    *p;

  register ssize_t
    i;

  /*
    Ellipses are just short segmented polys.
  */
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  primitive_info->coordinates=0;
  if ((fabs(radii.x) < MagickEpsilon) || (fabs(radii.y) < MagickEpsilon))
    return(MagickTrue);
  delta=2.0*PerceptibleReciprocal(MagickMax(radii.x,radii.y));
  step=MagickPI/8.0;
  if ((delta >= 0.0) && (delta < (MagickPI/8.0)))
    step=MagickPI/4.0/(MagickPI*PerceptibleReciprocal(delta)/2.0);
  angle.x=DegreesToRadians(arc.x);
  y=arc.y;
  while (y < arc.x)
    y+=360.0;
  angle.y=DegreesToRadians(y);
  coordinates=ceil((angle.y-angle.x)/step+1.0);
  if ((coordinates > (double) SSIZE_MAX) ||
      (coordinates > (double) GetMaxMemoryRequest()))
    {
      (void) ThrowMagickException(mvg_info->exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return(MagickFalse);
    }
  if (CheckPrimitiveExtent(mvg_info,(size_t) coordinates) == MagickFalse)
    return(MagickFalse);
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  for (p=primitive_info; angle.x < angle.y; angle.x+=step)
  {
    point.x=cos(fmod(angle.x,DegreesToRadians(360.0)))*radii.x+center.x;
    point.y=sin(fmod(angle.x,DegreesToRadians(360.0)))*radii.y+center.y;
    if (TracePoint(p,point) == MagickFalse)
      return(MagickFalse);
    p+=p->coordinates;
  }
  point.x=cos(fmod(angle.y,DegreesToRadians(360.0)))*radii.x+center.x;
  point.y=sin(fmod(angle.y,DegreesToRadians(360.0)))*radii.y+center.y;
  if (TracePoint(p,point) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  primitive_info->closed_subpath=MagickFalse;
  x=fabs(primitive_info[0].point.x-
    primitive_info[primitive_info->coordinates-1].point.x);
  y=fabs(primitive_info[0].point.y-
    primitive_info[primitive_info->coordinates-1].point.y);
  if ((x < MagickEpsilon) && (y < MagickEpsilon))
    primitive_info->closed_subpath=MagickTrue;
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  return(MagickTrue);
}