BGD_DECLARE(void) gdImageSaveAlpha (gdImagePtr im, int saveAlphaArg)
{
  im->saveAlphaFlag = saveAlphaArg;
}