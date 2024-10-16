void SplashOutputDev::endPage() {
  if (colorMode != splashModeMono1 && !keepAlphaChannel) {
    splash->compositeBackground(paperColor);
  }
}