static int zipfileReadData(
  FILE *pFile,                    /* Read from this file */
  u8 *aRead,                      /* Read into this buffer */
  int nRead,                      /* Number of bytes to read */
  i64 iOff,                       /* Offset to read from */
  char **pzErrmsg                 /* OUT: Error message (from sqlite3_malloc) */
){
  size_t n;
  fseek(pFile, (long)iOff, SEEK_SET);
  n = fread(aRead, 1, nRead, pFile);
  if( (int)n!=nRead ){
    *pzErrmsg = sqlite3_mprintf("error in fread()");
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
}