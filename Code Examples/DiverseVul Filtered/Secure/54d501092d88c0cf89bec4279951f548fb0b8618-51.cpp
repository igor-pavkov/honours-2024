static void zipfileTableErr(ZipfileTab *pTab, const char *zFmt, ...){
  va_list ap;
  va_start(ap, zFmt);
  sqlite3_free(pTab->base.zErrMsg);
  pTab->base.zErrMsg = sqlite3_vmprintf(zFmt, ap);
  va_end(ap);
}