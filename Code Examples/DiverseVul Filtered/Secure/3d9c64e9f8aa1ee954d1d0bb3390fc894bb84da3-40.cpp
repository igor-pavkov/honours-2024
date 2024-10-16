DLLEXPORT int tjDecompressHeader2(tjhandle handle, unsigned char *jpegBuf,
                                  unsigned long jpegSize, int *width,
                                  int *height, int *jpegSubsamp)
{
  int jpegColorspace;

  return tjDecompressHeader3(handle, jpegBuf, jpegSize, width, height,
                             jpegSubsamp, &jpegColorspace);
}