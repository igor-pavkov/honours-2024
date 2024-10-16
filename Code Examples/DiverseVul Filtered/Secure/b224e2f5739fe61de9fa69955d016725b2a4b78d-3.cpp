void SplashOutputDev::eoFill(GfxState *state) {
  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  SplashPath path = convertPath(state, state->getPath(), true);
  splash->fill(&path, true);
}