void SplashOutputDev::endTextObject(GfxState *state) {
  if (textClipPath) {
    splash->clipToPath(textClipPath, false);
    delete textClipPath;
    textClipPath = nullptr;
  }
}