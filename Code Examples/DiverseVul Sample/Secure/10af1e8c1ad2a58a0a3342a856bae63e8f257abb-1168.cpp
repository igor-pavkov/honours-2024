#else
    T* data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) {
      return _data + x + (ulongT)y*_width + (ulongT)z*_width*_height + (ulongT)c*_width*_height*_depth;