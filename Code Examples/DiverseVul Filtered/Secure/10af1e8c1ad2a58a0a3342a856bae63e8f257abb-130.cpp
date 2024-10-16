#if cimg_verbosity>=3
    CImg<T> *data(const unsigned int pos) {
      if (pos>=size())
        cimg::warn(_cimglist_instance
                   "data(): Invalid pointer request, at position [%u].",
                   cimglist_instance,
                   pos);
      return _data + pos;