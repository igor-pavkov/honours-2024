static int zipfileScanExtra(u8 *aExtra, int nExtra, u32 *pmTime){
  int ret = 0;
  u8 *p = aExtra;
  u8 *pEnd = &aExtra[nExtra];

  while( p<pEnd ){
    u16 id = zipfileRead16(p);
    u16 nByte = zipfileRead16(p);

    switch( id ){
      case ZIPFILE_EXTRA_TIMESTAMP: {
        u8 b = p[0];
        if( b & 0x01 ){     /* 0x01 -> modtime is present */
          *pmTime = zipfileGetU32(&p[1]);
          ret = 1;
        }
        break;
      }
    }

    p += nByte;
  }
  return ret;
}