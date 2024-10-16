    ByteVectorMirror mid(uint index, uint length = 0xffffffff) const
    {
      return length == 0xffffffff ? v.mid(0, index) : v.mid(index - length, length);
    }