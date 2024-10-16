void LineBitmapRequester::BuildCommon(void)
{
  UBYTE i;

  BitmapCtrl::BuildCommon();
  LineBuffer::BuildCommon();

  if (m_ppTempIBM == NULL) {
    m_ppTempIBM = (struct ImageBitMap **)m_pEnviron->AllocMem(sizeof(struct ImageBitMap **) * m_ucCount);
    memset(m_ppTempIBM,0,sizeof(struct ImageBitMap *) * m_ucCount);
  }

  if (m_pulReadyLines == NULL) {
    m_pulReadyLines = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulReadyLines,0,sizeof(ULONG) * m_ucCount);
  }

  if (m_pppImage == NULL) {
    m_pppImage    = (struct Line ***)m_pEnviron->AllocMem(sizeof(struct Line **) * m_ucCount);
    for(i = 0;i < m_ucCount;i++) {
       m_pppImage[i]         = m_ppTop + i;
    }
  }

  for(i = 0;i < m_ucCount;i++) {
    if (m_ppTempIBM[i] == NULL)
      m_ppTempIBM[i]      = new(m_pEnviron) struct ImageBitMap();
  }
}