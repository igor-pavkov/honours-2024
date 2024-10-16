DLLEXPORT int tjDecompressToYUV(tjhandle handle, unsigned char *jpegBuf,
                                unsigned long jpegSize, unsigned char *dstBuf,
                                int flags)
{
  return tjDecompressToYUV2(handle, jpegBuf, jpegSize, dstBuf, 0, 4, 0, flags);
}