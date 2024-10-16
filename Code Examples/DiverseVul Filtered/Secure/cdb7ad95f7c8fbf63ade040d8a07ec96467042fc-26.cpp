  bool operator()(const TrueTypeLoca &loca1, const TrueTypeLoca &loca2) {
    return loca1.idx < loca2.idx;
  }