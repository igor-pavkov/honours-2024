  ByteVector fromNumber(T value, bool mostSignificantByteFirst)
  {
    int size = sizeof(T);

    ByteVector v(size, 0);

    for(int i = 0; i < size; i++)
      v[i] = uchar(value >> ((mostSignificantByteFirst ? size - 1 - i : i) * 8) & 0xff);

    return v;
  }