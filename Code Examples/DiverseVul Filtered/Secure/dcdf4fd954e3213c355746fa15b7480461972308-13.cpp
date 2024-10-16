bool ByteVector::containsAt(const ByteVector &pattern, uint offset, uint patternOffset, uint patternLength) const
{
  if(pattern.size() < patternLength)
    patternLength = pattern.size();

  // do some sanity checking -- all of these things are needed for the search to be valid

  if(patternLength > size() || offset >= size() || patternOffset >= pattern.size() || patternLength == 0)
    return false;

  // loop through looking for a mismatch

  for(uint i = 0; i < patternLength - patternOffset; i++) {
    if(at(i + offset) != pattern[i + patternOffset])
      return false;
  }

  return true;
}