MagickExport MagickBooleanType StripImage(Image *image,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  (void) exception;
  DestroyImageProfiles(image);
  (void) DeleteImageProperty(image,"comment");
  (void) DeleteImageProperty(image,"date:create");
  (void) DeleteImageProperty(image,"date:modify");
  status=SetImageArtifact(image,"png:exclude-chunk",
    "bKGD,caNv,cHRM,eXIf,gAMA,iCCP,iTXt,pHYs,sRGB,tEXt,zCCP,zTXt,date");
  return(status);
}