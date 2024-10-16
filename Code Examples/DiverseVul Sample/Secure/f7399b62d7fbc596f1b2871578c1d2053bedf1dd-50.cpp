static int32_t readvec_signed(const std::vector<uint8_t>& data,int& ptr,int len)
{
  const uint32_t high_bit = 0x80<<((len-1)*8);

  uint32_t val=0;
  while (len--) {
    val <<= 8;
    val |= data[ptr++];
  }

  bool negative = (val & high_bit) != 0;
  val &= ~high_bit;

  if (negative) {
    return -(high_bit-val);
  }
  else {
    return val;
  }

  return val;
}