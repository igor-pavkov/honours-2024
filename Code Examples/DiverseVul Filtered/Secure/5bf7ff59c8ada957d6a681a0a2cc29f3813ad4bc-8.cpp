static inline void TraceBezierCurve(char *message,PointInfo *last,
  PointInfo *point)
{
  /*
    Handle special cases when Bezier curves are used to describe
    corners and straight lines.
  */
  if (((last+1)->x == (last+2)->x) && ((last+1)->y == (last+2)->y) &&
      (point->x == (point+1)->x) && (point->y == (point+1)->y))
    (void) FormatLocaleString(message,MagickPathExtent,
      "L %g %g\n",point[1].x,point[1].y);
  else
    (void) FormatLocaleString(message,MagickPathExtent,"C %g %g %g %g %g %g\n",
      (last+2)->x,(last+2)->y,point->x,point->y,(point+1)->x,(point+1)->y);
}