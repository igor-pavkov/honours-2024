static int DrawCompareEdges(const void *p_edge,const void *q_edge)
{
#define DrawCompareEdge(p,q) \
{ \
  if (((p)-(q)) < 0.0) \
    return(-1); \
  if (((p)-(q)) > 0.0) \
    return(1); \
}

  register const PointInfo
    *p,
    *q;

  /*
    Edge sorting for right-handed coordinate system.
  */
  p=((const EdgeInfo *) p_edge)->points;
  q=((const EdgeInfo *) q_edge)->points;
  DrawCompareEdge(p[0].y,q[0].y);
  DrawCompareEdge(p[0].x,q[0].x);
  DrawCompareEdge((p[1].x-p[0].x)*(q[1].y-q[0].y),(p[1].y-p[0].y)*
    (q[1].x-q[0].x));
  DrawCompareEdge(p[1].y,q[1].y);
  DrawCompareEdge(p[1].x,q[1].x);
  return(0);
}