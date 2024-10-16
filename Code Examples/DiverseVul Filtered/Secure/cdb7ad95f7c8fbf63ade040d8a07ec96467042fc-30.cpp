int FoFiTrueType::mapNameToGID(char *name) {
  if (!nameToGID) {
    return 0;
  }
  return nameToGID->lookupInt(name);
}