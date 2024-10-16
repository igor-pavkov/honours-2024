DLLEXPORT int tjEncodeYUV2(tjhandle handle, unsigned char *srcBuf, int width,
                           int pitch, int height, int pixelFormat,
                           unsigned char *dstBuf, int subsamp, int flags)
{
  return tjEncodeYUV3(handle, srcBuf, width, pitch, height, pixelFormat,
                      dstBuf, 4, subsamp, flags);
}