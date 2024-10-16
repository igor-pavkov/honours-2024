DLLEXPORT int tjCompress(tjhandle handle, unsigned char *srcBuf, int width,
                         int pitch, int height, int pixelSize,
                         unsigned char *jpegBuf, unsigned long *jpegSize,
                         int jpegSubsamp, int jpegQual, int flags)
{
  int retval = 0;
  unsigned long size;

  if (flags & TJ_YUV) {
    size = tjBufSizeYUV(width, height, jpegSubsamp);
    retval = tjEncodeYUV2(handle, srcBuf, width, pitch, height,
                          getPixelFormat(pixelSize, flags), jpegBuf,
                          jpegSubsamp, flags);
  } else {
    retval = tjCompress2(handle, srcBuf, width, pitch, height,
                         getPixelFormat(pixelSize, flags), &jpegBuf, &size,
                         jpegSubsamp, jpegQual, flags | TJFLAG_NOREALLOC);
  }
  *jpegSize = size;
  return retval;
}