static MagickBooleanType DrawBoundingRectangles(Image *image,
  const DrawInfo *draw_info,const PolygonInfo *polygon_info,
  ExceptionInfo *exception)
{
  double
    mid;

  DrawInfo
    *clone_info;

  MagickStatusType
    status;

  PointInfo
    end,
    resolution,
    start;

  PrimitiveInfo
    primitive_info[6];

  ssize_t
    i;

  SegmentInfo
    bounds;

  ssize_t
    coordinates;

  (void) memset(primitive_info,0,sizeof(primitive_info));
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  status=QueryColorCompliance("#000F",AllCompliance,&clone_info->fill,
    exception);
  if (status == MagickFalse)
    {
      clone_info=DestroyDrawInfo(clone_info);
      return(MagickFalse);
    }
  resolution.x=96.0;
  resolution.y=96.0;
  if (clone_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(clone_info->density,&geometry_info);
      if ((flags & RhoValue) != 0)
        resolution.x=geometry_info.rho;
      resolution.y=resolution.x;
      if ((flags & SigmaValue) != 0)
        resolution.y=geometry_info.sigma;
    }
  mid=(resolution.x/96.0)*ExpandAffine(&clone_info->affine)*
    clone_info->stroke_width/2.0;
  bounds.x1=0.0;
  bounds.y1=0.0;
  bounds.x2=0.0;
  bounds.y2=0.0;
  if (polygon_info != (PolygonInfo *) NULL)
    {
      bounds=polygon_info->edges[0].bounds;
      for (i=1; i < (ssize_t) polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].bounds.x1 < (double) bounds.x1)
          bounds.x1=polygon_info->edges[i].bounds.x1;
        if (polygon_info->edges[i].bounds.y1 < (double) bounds.y1)
          bounds.y1=polygon_info->edges[i].bounds.y1;
        if (polygon_info->edges[i].bounds.x2 > (double) bounds.x2)
          bounds.x2=polygon_info->edges[i].bounds.x2;
        if (polygon_info->edges[i].bounds.y2 > (double) bounds.y2)
          bounds.y2=polygon_info->edges[i].bounds.y2;
      }
      bounds.x1-=mid;
      bounds.x1=bounds.x1 < 0.0 ? 0.0 : bounds.x1 >= (double)
        image->columns ? (double) image->columns-1 : bounds.x1;
      bounds.y1-=mid;
      bounds.y1=bounds.y1 < 0.0 ? 0.0 : bounds.y1 >= (double)
        image->rows ? (double) image->rows-1 : bounds.y1;
      bounds.x2+=mid;
      bounds.x2=bounds.x2 < 0.0 ? 0.0 : bounds.x2 >= (double)
        image->columns ? (double) image->columns-1 : bounds.x2;
      bounds.y2+=mid;
      bounds.y2=bounds.y2 < 0.0 ? 0.0 : bounds.y2 >= (double)
        image->rows ? (double) image->rows-1 : bounds.y2;
      for (i=0; i < (ssize_t) polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].direction != 0)
          status=QueryColorCompliance("#f00",AllCompliance,&clone_info->stroke,
            exception);
        else
          status=QueryColorCompliance("#0f0",AllCompliance,&clone_info->stroke,
            exception);
        if (status == MagickFalse)
          break;
        start.x=(double) (polygon_info->edges[i].bounds.x1-mid);
        start.y=(double) (polygon_info->edges[i].bounds.y1-mid);
        end.x=(double) (polygon_info->edges[i].bounds.x2+mid);
        end.y=(double) (polygon_info->edges[i].bounds.y2+mid);
        primitive_info[0].primitive=RectanglePrimitive;
        status&=TraceRectangle(primitive_info,start,end);
        primitive_info[0].method=ReplaceMethod;
        coordinates=(ssize_t) primitive_info[0].coordinates;
        primitive_info[coordinates].primitive=UndefinedPrimitive;
        status=DrawPrimitive(image,clone_info,primitive_info,exception);
        if (status == MagickFalse)
          break;
      }
      if (i < (ssize_t) polygon_info->number_edges)
        {
          clone_info=DestroyDrawInfo(clone_info);
          return(status == 0 ? MagickFalse : MagickTrue);
        }
    }
  status=QueryColorCompliance("#00f",AllCompliance,&clone_info->stroke,
    exception);
  if (status == MagickFalse)
    {
      clone_info=DestroyDrawInfo(clone_info);
      return(MagickFalse);
    }
  start.x=(double) (bounds.x1-mid);
  start.y=(double) (bounds.y1-mid);
  end.x=(double) (bounds.x2+mid);
  end.y=(double) (bounds.y2+mid);
  primitive_info[0].primitive=RectanglePrimitive;
  status&=TraceRectangle(primitive_info,start,end);
  primitive_info[0].method=ReplaceMethod;
  coordinates=(ssize_t) primitive_info[0].coordinates;
  primitive_info[coordinates].primitive=UndefinedPrimitive;
  status=DrawPrimitive(image,clone_info,primitive_info,exception);
  clone_info=DestroyDrawInfo(clone_info);
  return(status == 0 ? MagickFalse : MagickTrue);
}