static CustomStreamInfo *TIFFAcquireCustomStreamForReading(
  PhotoshopProfile *profile,ExceptionInfo *exception)
{
  CustomStreamInfo
    *custom_stream;

  custom_stream=AcquireCustomStreamInfo(exception);
  if (custom_stream == (CustomStreamInfo *) NULL)
    return(custom_stream);
  SetCustomStreamData(custom_stream,(void *) profile);
  SetCustomStreamReader(custom_stream,TIFFReadCustomStream);
  SetCustomStreamSeeker(custom_stream,TIFFSeekCustomStream);
  SetCustomStreamTeller(custom_stream,TIFFTellCustomStream);
  return(custom_stream);
}