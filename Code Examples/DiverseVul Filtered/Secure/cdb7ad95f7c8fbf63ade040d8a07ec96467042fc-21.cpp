int FoFiTrueType::getEmbeddingRights() {
  int i, fsType;
  GBool ok;

  if ((i = seekTable("OS/2")) < 0) {
    return 4;
  }
  ok = gTrue;
  fsType = getU16BE(tables[i].offset + 8, &ok);
  if (!ok) {
    return 4;
  }
  if (fsType & 0x0008) {
    return 2;
  }
  if (fsType & 0x0004) {
    return 1;
  }
  if (fsType & 0x0002) {
    return 0;
  }
  return 3;
}