#if cimg_verbosity>=3
    T& operator()(const unsigned int x, const unsigned int y=0,
                  const unsigned int z=0, const unsigned int c=0) {
      const ulongT off = (ulongT)offset(x,y,z,c);
      if (!_data || off>=size()) {
        cimg::warn(_cimg_instance
                   "operator(): Invalid pixel request, at coordinates (%d,%d,%d,%d) [offset=%u].",
                   cimg_instance,
                   (int)x,(int)y,(int)z,(int)c,off);
        return *_data;
      }
      else return _data[off];