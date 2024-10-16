DLLEXPORT int tjEncodeYUV(tjhandle handle, unsigned char *srcBuf, int width,
                          int pitch, int height, int pixelSize,
                          unsigned char *dstBuf, int subsamp, int flags)
{
  return tjEncodeYUV2(handle, srcBuf, width, pitch, height,
                      getPixelFormat(pixelSize, flags), dstBuf, subsamp,
                      flags);
}