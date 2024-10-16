DLLEXPORT int tjDecompress(tjhandle handle, unsigned char *jpegBuf,
                           unsigned long jpegSize, unsigned char *dstBuf,
                           int width, int pitch, int height, int pixelSize,
                           int flags)
{
  if (flags & TJ_YUV)
    return tjDecompressToYUV(handle, jpegBuf, jpegSize, dstBuf, flags);
  else
    return tjDecompress2(handle, jpegBuf, jpegSize, dstBuf, width, pitch,
                         height, getPixelFormat(pixelSize, flags), flags);
}