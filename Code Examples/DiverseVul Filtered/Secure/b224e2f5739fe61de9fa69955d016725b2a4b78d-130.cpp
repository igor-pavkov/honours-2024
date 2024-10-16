void SplashOutputDev::setupScreenParams(double hDPI, double vDPI) {
  screenParams.size = -1;
  screenParams.dotRadius = -1;
  screenParams.gamma = (SplashCoord)1.0;
  screenParams.blackThreshold = (SplashCoord)0.0;
  screenParams.whiteThreshold = (SplashCoord)1.0;

  // use clustered dithering for resolution >= 300 dpi
  // (compare to 299.9 to avoid floating point issues)
  if (hDPI > 299.9 && vDPI > 299.9) {
    screenParams.type = splashScreenStochasticClustered;
    if (screenParams.size < 0) {
      screenParams.size = 64;
    }
    if (screenParams.dotRadius < 0) {
      screenParams.dotRadius = 2;
    }
  } else {
    screenParams.type = splashScreenDispersed;
    if (screenParams.size < 0) {
      screenParams.size = 4;
    }
  }
}