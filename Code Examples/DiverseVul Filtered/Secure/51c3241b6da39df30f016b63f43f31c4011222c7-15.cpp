void LineBitmapRequester::RequestUserDataForEncoding(class BitMapHook *bmh,RectAngle<LONG> &region,bool alpha)
{ 
  int i;

  m_ulMaxMCU = MAX_ULONG;
  
  for(i = 0;i < m_ucCount;i++) {
    ULONG max;
    //
    // Components are always requested completely on encoding.
    RequestUserData(bmh,region,i,alpha);
    // All components must have the same sample precision here.
    max = (m_ppBitmap[i]->ibm_ulHeight - 1) >> 3;
    if (max < m_ulMaxMCU)
      m_ulMaxMCU = max; 
    if (LONG(m_ppBitmap[i]->ibm_ulHeight) - 1 < region.ra_MaxY)
      region.ra_MaxY = m_ppBitmap[i]->ibm_ulHeight - 1;
  }
}