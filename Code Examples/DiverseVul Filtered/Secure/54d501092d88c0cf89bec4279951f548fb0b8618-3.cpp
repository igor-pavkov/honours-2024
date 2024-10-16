static void zipfileInflate(
  sqlite3_context *pCtx,          /* Store result here */
  const u8 *aIn,                  /* Compressed data */
  int nIn,                        /* Size of buffer aIn[] in bytes */
  int nOut                        /* Expected output size */
){
  u8 *aRes = sqlite3_malloc(nOut);
  if( aRes==0 ){
    sqlite3_result_error_nomem(pCtx);
  }else{
    int err;
    z_stream str;
    memset(&str, 0, sizeof(str));

    str.next_in = (Byte*)aIn;
    str.avail_in = nIn;
    str.next_out = (Byte*)aRes;
    str.avail_out = nOut;

    err = inflateInit2(&str, -15);
    if( err!=Z_OK ){
      zipfileCtxErrorMsg(pCtx, "inflateInit2() failed (%d)", err);
    }else{
      err = inflate(&str, Z_NO_FLUSH);
      if( err!=Z_STREAM_END ){
        zipfileCtxErrorMsg(pCtx, "inflate() failed (%d)", err);
      }else{
        sqlite3_result_blob(pCtx, aRes, nOut, zipfileFree);
        aRes = 0;
      }
    }
    sqlite3_free(aRes);
    inflateEnd(&str);
  }
}