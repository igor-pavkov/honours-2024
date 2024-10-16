static void zipfileCursorErr(ZipfileCsr *pCsr, const char *zFmt, ...){
  va_list ap;
  va_start(ap, zFmt);
  sqlite3_free(pCsr->base.pVtab->zErrMsg);
  pCsr->base.pVtab->zErrMsg = sqlite3_vmprintf(zFmt, ap);
  va_end(ap);
}