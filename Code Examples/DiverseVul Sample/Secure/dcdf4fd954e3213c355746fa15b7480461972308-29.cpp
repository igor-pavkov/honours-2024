int ByteVector::endsWithPartialMatch(const ByteVector &pattern) const
{
  if(pattern.size() > size())
    return -1;

  const int startIndex = size() - pattern.size();

  // try to match the last n-1 bytes from the vector (where n is the pattern
  // size) -- continue trying to match n-2, n-3...1 bytes

  for(uint i = 1; i < pattern.size(); i++) {
    if(containsAt(pattern, startIndex + i, 0, pattern.size() - i))
      return startIndex + i;
  }

  return -1;
}