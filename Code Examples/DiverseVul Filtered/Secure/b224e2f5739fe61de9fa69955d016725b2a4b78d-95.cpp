void SplashOutputDev::clipToStrokePath(GfxState *state) {
  SplashPath *path2;

  SplashPath path = convertPath(state, state->getPath(), false);
  path2 = splash->makeStrokePath(&path, state->getLineWidth());
  splash->clipToPath(path2, false);
  delete path2;
}