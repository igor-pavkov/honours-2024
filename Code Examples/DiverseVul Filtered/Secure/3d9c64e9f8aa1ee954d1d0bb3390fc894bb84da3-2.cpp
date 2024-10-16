static int getPixelFormat(int pixelSize, int flags)
{
  if (pixelSize == 1) return TJPF_GRAY;
  if (pixelSize == 3) {
    if (flags & TJ_BGR) return TJPF_BGR;
    else return TJPF_RGB;
  }
  if (pixelSize == 4) {
    if (flags & TJ_ALPHAFIRST) {
      if (flags & TJ_BGR) return TJPF_XBGR;
      else return TJPF_XRGB;
    } else {
      if (flags & TJ_BGR) return TJPF_BGRX;
      else return TJPF_RGBX;
    }
  }
  return -1;
}