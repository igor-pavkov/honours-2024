  uint16_t getID()
  {
    uint16_t ret=peakID();
    d_available.pop_front();
    return ret;
  }