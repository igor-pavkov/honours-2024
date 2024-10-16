  bool operator()(const TrueTypeTable &tab1, const TrueTypeTable &tab2) {
    return tab1.tag < tab2.tag;
  }