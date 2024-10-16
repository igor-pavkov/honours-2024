static u32 zipfileTime(void){
  sqlite3_vfs *pVfs = sqlite3_vfs_find(0);
  u32 ret;
  if( pVfs->iVersion>=2 && pVfs->xCurrentTimeInt64 ){
    i64 ms;
    pVfs->xCurrentTimeInt64(pVfs, &ms);
    ret = (u32)((ms/1000) - ((i64)24405875 * 8640));
  }else{
    double day;
    pVfs->xCurrentTime(pVfs, &day);
    ret = (u32)((day - 2440587.5) * 86400);
  }
  return ret;
}