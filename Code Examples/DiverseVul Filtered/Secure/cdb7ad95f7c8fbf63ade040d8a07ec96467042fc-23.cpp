  bool operator()(const TrueTypeLoca &loca1, const TrueTypeLoca &loca2) {
    if (loca1.origOffset == loca2.origOffset) {
      return loca1.idx < loca2.idx;
    }
    return loca1.origOffset < loca2.origOffset;
  }