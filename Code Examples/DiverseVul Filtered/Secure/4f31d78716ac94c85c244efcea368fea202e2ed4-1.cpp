static inline void CompositeCanvas(Image *destination,
  const CompositeOperator compose, Image *source,ssize_t x_offset,
  ssize_t y_offset )
{
  x_offset+=source->page.x-destination->page.x;
  y_offset+=source->page.y-destination->page.y;
  (void) CompositeImage(destination,compose,source,x_offset,y_offset);
}