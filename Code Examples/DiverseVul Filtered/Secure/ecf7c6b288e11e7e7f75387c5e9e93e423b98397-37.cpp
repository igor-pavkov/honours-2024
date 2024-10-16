static inline double GetStopColorOffset(const GradientInfo *gradient,
  const ssize_t x,const ssize_t y)
{
  switch (gradient->type)
  {
    case UndefinedGradient:
    case LinearGradient:
    {
      double
        gamma,
        length,
        offset,
        scale;

      PointInfo
        p,
        q;

      const SegmentInfo
        *gradient_vector;

      gradient_vector=(&gradient->gradient_vector);
      p.x=gradient_vector->x2-gradient_vector->x1;
      p.y=gradient_vector->y2-gradient_vector->y1;
      q.x=(double) x-gradient_vector->x1;
      q.y=(double) y-gradient_vector->y1;
      length=sqrt(q.x*q.x+q.y*q.y);
      gamma=sqrt(p.x*p.x+p.y*p.y)*length;
      gamma=PerceptibleReciprocal(gamma);
      scale=p.x*q.x+p.y*q.y;
      offset=gamma*scale*length;
      return(offset);
    }
    case RadialGradient:
    {
      PointInfo
        v;

      if (gradient->spread == RepeatSpread)
        {
          v.x=(double) x-gradient->center.x;
          v.y=(double) y-gradient->center.y;
          return(sqrt(v.x*v.x+v.y*v.y));
        }
      v.x=(double) (((x-gradient->center.x)*cos(DegreesToRadians(
        gradient->angle)))+((y-gradient->center.y)*sin(DegreesToRadians(
        gradient->angle))))*PerceptibleReciprocal(gradient->radii.x);
      v.y=(double) (((x-gradient->center.x)*sin(DegreesToRadians(
        gradient->angle)))-((y-gradient->center.y)*cos(DegreesToRadians(
        gradient->angle))))*PerceptibleReciprocal(gradient->radii.y);
      return(sqrt(v.x*v.x+v.y*v.y));
    }
  }
  return(0.0);
}