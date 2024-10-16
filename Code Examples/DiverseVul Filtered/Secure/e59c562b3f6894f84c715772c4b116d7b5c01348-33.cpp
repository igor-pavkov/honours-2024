void sqlite3SelectDestInit(SelectDest *pDest, int eDest, int iParm){
  pDest->eDest = (u8)eDest;
  pDest->iSDParm = iParm;
  pDest->zAffSdst = 0;
  pDest->iSdst = 0;
  pDest->nSdst = 0;
}