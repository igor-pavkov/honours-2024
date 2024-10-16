WandExport void DrawPathMoveToRelative(DrawingWand *wand,const double x,
  const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathMoveTo(wand,RelativePathMode,x,y);
}