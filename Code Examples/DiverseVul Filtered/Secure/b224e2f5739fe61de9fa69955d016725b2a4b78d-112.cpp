void SplashOutputDev::updateTransfer(GfxState *state) {
  Function **transfer;
  unsigned char red[256], green[256], blue[256], gray[256];
  double x, y;
  int i;

  transfer = state->getTransfer();
  if (transfer[0] &&
      transfer[0]->getInputSize() == 1 &&
      transfer[0]->getOutputSize() == 1) {
    if (transfer[1] &&
	transfer[1]->getInputSize() == 1 &&
	transfer[1]->getOutputSize() == 1 &&
	transfer[2] &&
	transfer[2]->getInputSize() == 1 &&
	transfer[2]->getOutputSize() == 1 &&
	transfer[3] &&
	transfer[3]->getInputSize() == 1 &&
	transfer[3]->getOutputSize() == 1) {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = (unsigned char)(y * 255.0 + 0.5);
	transfer[1]->transform(&x, &y);
	green[i] = (unsigned char)(y * 255.0 + 0.5);
	transfer[2]->transform(&x, &y);
	blue[i] = (unsigned char)(y * 255.0 + 0.5);
	transfer[3]->transform(&x, &y);
	gray[i] = (unsigned char)(y * 255.0 + 0.5);
      }
    } else {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = green[i] = blue[i] = gray[i] = (unsigned char)(y * 255.0 + 0.5);
      }
    }
  } else {
    for (i = 0; i < 256; ++i) {
      red[i] = green[i] = blue[i] = gray[i] = (unsigned char)i;
    }
  }
  splash->setTransfer(red, green, blue, gray);
}