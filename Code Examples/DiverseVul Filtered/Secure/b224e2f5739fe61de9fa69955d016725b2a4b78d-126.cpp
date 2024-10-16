void SplashOutputDev::stroke(GfxState *state) {
  if (state->getStrokeColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getStrokeColorSpace(), state->getStrokeOverprint(),
		   state->getOverprintMode(), state->getStrokeColor());
  SplashPath path = convertPath(state, state->getPath(), false);
  splash->stroke(&path);
}