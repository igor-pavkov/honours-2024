bool constantTimeEquals(const string& left, const string& right)
{
  if (left.size() != right.size()) {
    return false;
  }

  unsigned valid = 0;
  for (size_t i = 0; i < left.size(); ++i) {
    valid |= left[i] ^ right[i];
  }

  return valid == 0;
}