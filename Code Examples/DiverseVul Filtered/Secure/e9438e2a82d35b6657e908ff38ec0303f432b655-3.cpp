static int CMSExceptionHandler(int severity,const char *message)
{
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),"lcms: #%d, %s",
    severity,message != (char *) NULL ? message : "no message");
  return(1);
}