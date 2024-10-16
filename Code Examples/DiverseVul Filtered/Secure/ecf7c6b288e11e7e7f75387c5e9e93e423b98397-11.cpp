static MagickBooleanType DrawRoundLinecap(Image *image,
  const DrawInfo *draw_info,const PrimitiveInfo *primitive_info,
  ExceptionInfo *exception)
{
  PrimitiveInfo
    linecap[5];

  register ssize_t
    i;

  for (i=0; i < 4; i++)
    linecap[i]=(*primitive_info);
  linecap[0].coordinates=4;
  linecap[1].point.x+=2.0*MagickEpsilon;
  linecap[2].point.x+=2.0*MagickEpsilon;
  linecap[2].point.y+=2.0*MagickEpsilon;
  linecap[3].point.y+=2.0*MagickEpsilon;
  linecap[4].primitive=UndefinedPrimitive;
  return(DrawPolygonPrimitive(image,draw_info,linecap,exception));
}