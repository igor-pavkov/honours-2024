void LineBitmapRequester::RequestUserDataForDecoding(class BitMapHook *bmh,RectAngle<LONG> &region,
                                                     const struct RectangleRequest *rr,bool alpha)
{ 
  int i;

  ResetBitmaps();
  
  m_ulMaxMCU = MAX_ULONG;
  
  for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
    RequestUserData(bmh,region,i,alpha);
    ULONG max = (BitmapOf(i).ibm_ulHeight >> 3) - 1;
    if (max < m_ulMaxMCU)
      m_ulMaxMCU = max;
  }
}