int sqlite3_vtab_in(sqlite3_index_info *pIdxInfo, int iCons, int bHandle){
  HiddenIndexInfo *pHidden = (HiddenIndexInfo*)&pIdxInfo[1];
  u32 m = SMASKBIT32(iCons);
  if( m & pHidden->mIn ){
    if( bHandle==0 ){ 
      pHidden->mHandleIn &= ~m;
    }else if( bHandle>0 ){
      pHidden->mHandleIn |= m;
    }
    return 1;
  }
  return 0;
}