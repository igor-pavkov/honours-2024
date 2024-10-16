static inline double SaneStrokeWidth(const Image *image,
  const DrawInfo *draw_info)
{
  return(MagickMin((double) draw_info->stroke_width,
    (2.0*sqrt(2.0)+MagickEpsilon)*MagickMax(image->columns,image->rows)));
}