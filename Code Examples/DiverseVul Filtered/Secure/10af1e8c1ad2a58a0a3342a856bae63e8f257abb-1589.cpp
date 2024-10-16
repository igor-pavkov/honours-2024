#if cimg_verbosity>=3
    T *data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) {
      const ulongT off = (ulongT)offset(x,y,z,c);
      if (off>=size())
        cimg::warn(_cimg_instance
                   "data(): Invalid pointer request, at coordinates (%u,%u,%u,%u) [offset=%u].",
                   cimg_instance,
                   x,y,z,c,off);
      return _data + off;