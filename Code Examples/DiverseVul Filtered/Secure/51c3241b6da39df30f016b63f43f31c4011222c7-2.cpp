void LineBitmapRequester::Next8Lines(UBYTE c)
{
  int cnt = 8;
  do {
    struct Line *row = *m_pppImage[c];
    if (!row)
      break;
    m_pppImage[c] = &(row->m_pNext);
  } while(--cnt && *m_pppImage[c]);
}