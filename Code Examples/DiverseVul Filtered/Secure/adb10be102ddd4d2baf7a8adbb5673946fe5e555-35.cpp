  void skipLabel()
  {
    uint8_t len; 
    while((len=get8BitInt())) { 
      if(len >= 0xc0) { // extended label
        get8BitInt();
        return;
      }
      skipBytes(len);
    }
  }