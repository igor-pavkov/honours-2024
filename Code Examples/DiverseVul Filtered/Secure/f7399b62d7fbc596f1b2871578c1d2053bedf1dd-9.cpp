static uint32_t readvec(const std::vector<uint8_t>& data,int& ptr,int len)
{
  uint32_t val=0;
  while (len--) {
    val <<= 8;
    val |= data[ptr++];
  }

  return val;
}