  uint16_t peakID()
  {
    uint16_t ret;
    if(!d_available.empty()) {
      ret=d_available.front();
      return ret;
    }
    else
      throw runtime_error("out of ids!"); // XXX FIXME
  }