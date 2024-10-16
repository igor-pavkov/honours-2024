bool LineBitmapRequester::isNextMCULineReady(void) const
{
  int i;

  for(i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < m_ulPixelHeight) { // There is still data to encode
      class Component *comp = m_pFrame->ComponentOf(i);
      ULONG codedlines      = m_pulCurrentY[i] * comp->SubYOf();
      // codedlines + comp->SubYOf() << 3 * comp->MCUHeightOf() is the number of 
      // lines that must be buffered to encode the next MCU
      if (m_pulReadyLines[i] < codedlines + (comp->SubYOf() << 3) * comp->MCUHeightOf())
        return false;
    }
  }

  return true;
}