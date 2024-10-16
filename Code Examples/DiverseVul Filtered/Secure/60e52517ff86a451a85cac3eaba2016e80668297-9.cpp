static CustomStreamInfo *TIFFAcquireCustomStreamForWriting(
  PhotoshopProfile *profile,ExceptionInfo *exception)
{
  CustomStreamInfo
    *custom_stream;

  custom_stream=AcquireCustomStreamInfo(exception);
  if (custom_stream == (CustomStreamInfo *) NULL)
    return(custom_stream);
  SetCustomStreamData(custom_stream,(void *) profile);
  SetCustomStreamWriter(custom_stream,TIFFWriteCustomStream);
  SetCustomStreamSeeker(custom_stream,TIFFSeekCustomStream);
  SetCustomStreamTeller(custom_stream,TIFFTellCustomStream);
  return(custom_stream);
}