int FoFiTrueType::findCmap(int platform, int encoding) {
  int i;

  for (i = 0; i < nCmaps; ++i) {
    if (cmaps[i].platform == platform && cmaps[i].encoding == encoding) {
      return i;
    }
  }
  return -1;
}