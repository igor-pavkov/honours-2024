  T toNumber(const std::vector<char> &data, bool mostSignificantByteFirst)
  {
    T sum = 0;

    if(data.size() <= 0) {
      debug("ByteVectorMirror::toNumber<T>() -- data is empty, returning 0");
      return sum;
    }

    uint size = sizeof(T);
    uint last = data.size() > size ? size - 1 : data.size() - 1;

    for(uint i = 0; i <= last; i++)
      sum |= (T) uchar(data[i]) << ((mostSignificantByteFirst ? last - i : i) * 8);

    return sum;
  }