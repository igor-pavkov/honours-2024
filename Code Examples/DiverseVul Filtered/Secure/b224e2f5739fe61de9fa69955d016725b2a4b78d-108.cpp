void SplashOutputDev::setVectorAntialias(bool vaa) {
  vaa = vaa && colorMode != splashModeMono1;
  vectorAntialias = vaa;
  splash->setVectorAntialias(vaa);
}