BGD_DECLARE(void) gdImageAlphaBlending (gdImagePtr im, int alphaBlendingArg)
{
  im->alphaBlendingFlag = alphaBlendingArg;
}