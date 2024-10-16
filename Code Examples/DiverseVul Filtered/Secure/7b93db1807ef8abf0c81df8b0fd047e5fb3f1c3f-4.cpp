static MagickBooleanType DecodeImage(Image *image, unsigned char *pixels,
  const size_t length)
{
#define RLE_MODE_NONE -1
#define RLE_MODE_COPY  0
#define RLE_MODE_RUN   1

  int           data = 0, count = 0;
  unsigned char *p;
  int           mode = RLE_MODE_NONE;

  for (p = pixels; p < pixels + length; p++) {
    if (0 == count) {
      data = ReadBlobByte( image );
      if (-1 == data) return MagickFalse;
      if (data > 128) {
        mode  = RLE_MODE_RUN;
        count = data - 128 + 1;
        data  = ReadBlobByte( image );
        if (-1 == data) return MagickFalse;
      } else {
        mode  = RLE_MODE_COPY;
        count = data + 1;
      }
    }

    if (RLE_MODE_COPY == mode) {
      data = ReadBlobByte( image );
      if (-1 == data) return MagickFalse;
    }
    *p = (unsigned char)data;
    --count;
  }
  return MagickTrue;
}