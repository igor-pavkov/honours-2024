struct Line *LineBitmapRequester::Start8Lines(UBYTE c)
{
  if (*m_pppImage[c] == NULL) {
    struct Line **target = m_pppImage[c];
    int cnt = 8;
    do {
      *target = new(m_pEnviron) struct Line;
      (*target)->m_pData = (LONG *)m_pEnviron->AllocMem(m_pulWidth[c] * sizeof(LONG));
      target  = &((*target)->m_pNext);
    } while(--cnt);
  }
  return *m_pppImage[c];
}