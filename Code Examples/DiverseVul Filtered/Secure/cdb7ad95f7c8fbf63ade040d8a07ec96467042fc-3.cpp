Guint FoFiTrueType::charToTag(const char *tagName)
{
  int n = strlen(tagName);
  Guint tag = 0;
  int i;

  if (n > 4) n = 4;
  for (i = 0;i < n;i++) {
    tag <<= 8;
    tag |= tagName[i] & 0xff;
  }
  for (;i < 4;i++) {
    tag <<= 8;
    tag |= ' ';
  }
  return tag;
}