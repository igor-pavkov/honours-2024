static void DrawPathLineTo(DrawingWand *wand,const PathMode mode,
  const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathLineToOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathLineToOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand,"%c%.20g %.20g",mode == AbsolutePathMode ?
        'L' : 'l',x,y);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g %.20g",x,y);
}