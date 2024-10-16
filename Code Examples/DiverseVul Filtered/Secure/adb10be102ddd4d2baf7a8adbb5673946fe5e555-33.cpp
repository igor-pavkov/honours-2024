  void skipRData()
  {
    int toskip = get16BitInt();
    moveOffset(toskip);
  }