BGD_DECLARE(void) gdImageInterlace (gdImagePtr im, int interlaceArg)
{
  im->interlace = interlaceArg;
}