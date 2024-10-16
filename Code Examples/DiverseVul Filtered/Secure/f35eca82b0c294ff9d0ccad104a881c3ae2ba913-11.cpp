MagickExport void SetGeometryInfo(GeometryInfo *geometry_info)
{
  assert(geometry_info != (GeometryInfo *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  (void) memset(geometry_info,0,sizeof(*geometry_info));
}