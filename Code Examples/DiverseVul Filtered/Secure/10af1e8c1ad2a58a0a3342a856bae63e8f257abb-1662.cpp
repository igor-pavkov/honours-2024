#else
    T& operator()(const unsigned int x) {
      return _data[x];