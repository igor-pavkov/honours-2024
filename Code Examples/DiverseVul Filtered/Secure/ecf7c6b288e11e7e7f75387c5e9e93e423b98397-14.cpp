static PathInfo *ConvertPrimitiveToPath(const PrimitiveInfo *primitive_info)
{
  MagickBooleanType
    closed_subpath;

  PathInfo
    *path_info;

  PathInfoCode
    code;

  PointInfo
    p,
    q;

  register ssize_t
    i,
    n;

  ssize_t
    coordinates,
    start;

  /*
    Converts a PrimitiveInfo structure into a vector path structure.
  */
  switch (primitive_info->primitive)
  {
    case AlphaPrimitive:
    case ColorPrimitive:
    case ImagePrimitive:
    case PointPrimitive:
    case TextPrimitive:
      return((PathInfo *) NULL);
    default:
      break;
  }
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++) ;
  path_info=(PathInfo *) AcquireQuantumMemory((size_t) (3UL*i+1UL),
    sizeof(*path_info));
  if (path_info == (PathInfo *) NULL)
    return((PathInfo *) NULL);
  coordinates=0;
  closed_subpath=MagickFalse;
  n=0;
  p.x=(-1.0);
  p.y=(-1.0);
  q.x=(-1.0);
  q.y=(-1.0);
  start=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    code=LineToCode;
    if (coordinates <= 0)
      {
        /*
          New subpath.
        */
        coordinates=(ssize_t) primitive_info[i].coordinates;
        p=primitive_info[i].point;
        start=n;
        code=MoveToCode;
        closed_subpath=primitive_info[i].closed_subpath;
      }
    coordinates--;
    if ((code == MoveToCode) || (coordinates <= 0) ||
        (fabs(q.x-primitive_info[i].point.x) >= MagickEpsilon) ||
        (fabs(q.y-primitive_info[i].point.y) >= MagickEpsilon))
      {
        /*
          Eliminate duplicate points.
        */
        path_info[n].code=code;
        path_info[n].point=primitive_info[i].point;
        q=primitive_info[i].point;
        n++;
      }
    if (coordinates > 0)
      continue;  /* next point in current subpath */
    if (closed_subpath != MagickFalse)
      {
        closed_subpath=MagickFalse;
        continue;
      }
    /*
      Mark the p point as open if the subpath is not closed.
    */
    path_info[start].code=OpenCode;
    path_info[n].code=GhostlineCode;
    path_info[n].point=primitive_info[i].point;
    n++;
    path_info[n].code=LineToCode;
    path_info[n].point=p;
    n++;
  }
  path_info[n].code=EndCode;
  path_info[n].point.x=0.0;
  path_info[n].point.y=0.0;
  if (IsEventLogging() != MagickFalse)
    LogPathInfo(path_info);
  path_info=(PathInfo *) ResizeQuantumMemory(path_info,(size_t) (n+1),
    sizeof(*path_info));
  return(path_info);
}