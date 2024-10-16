void LineBitmapRequester::ResetToStartOfImage(void)
{
  for(UBYTE i = 0;i < m_ucCount;i++) {
    m_pppImage[i]      = &m_ppTop[i];
    m_pulReadyLines[i] = 0;
  }
}