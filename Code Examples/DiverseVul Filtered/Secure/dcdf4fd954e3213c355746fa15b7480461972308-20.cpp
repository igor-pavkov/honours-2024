  int vectorFind(const Vector &v, const Vector &pattern, uint offset, int byteAlign)
  {
    if(pattern.size() > v.size() || offset > v.size() - 1)
      return -1;

    // Let's go ahead and special case a pattern of size one since that's common
    // and easy to make fast.

    if(pattern.size() == 1) {
      char p = pattern[0];
      for(uint i = offset; i < v.size(); i++) {
        if(v[i] == p && (i - offset) % byteAlign == 0)
          return i;
      }
      return -1;
    }

    uchar lastOccurrence[256];

    for(uint i = 0; i < 256; ++i)
      lastOccurrence[i] = uchar(pattern.size());

    for(uint i = 0; i < pattern.size() - 1; ++i)
      lastOccurrence[uchar(pattern[i])] = uchar(pattern.size() - i - 1);

    for(uint i = pattern.size() - 1 + offset; i < v.size(); i += lastOccurrence[uchar(v.at(i))]) {
      int iBuffer = i;
      int iPattern = pattern.size() - 1;

      while(iPattern >= 0 && v.at(iBuffer) == pattern[iPattern]) {
        --iBuffer;
        --iPattern;
      }

      if(-1 == iPattern && (iBuffer + 1 - offset) % byteAlign == 0)
        return iBuffer + 1;
    }

    return -1;
  }