DLLEXPORT int tjDecompressHeader(tjhandle handle, unsigned char *jpegBuf,
                                 unsigned long jpegSize, int *width,
                                 int *height)
{
  int jpegSubsamp;

  return tjDecompressHeader2(handle, jpegBuf, jpegSize, width, height,
                             &jpegSubsamp);
}