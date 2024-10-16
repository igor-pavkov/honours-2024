static void explainSimpleCount(
  Parse *pParse,                  /* Parse context */
  Table *pTab,                    /* Table being queried */
  Index *pIdx                     /* Index used to optimize scan, or NULL */
){
  if( pParse->explain==2 ){
    int bCover = (pIdx!=0 && (HasRowid(pTab) || !IsPrimaryKeyIndex(pIdx)));
    sqlite3VdbeExplain(pParse, 0, "SCAN TABLE %s%s%s",
        pTab->zName,
        bCover ? " USING COVERING INDEX " : "",
        bCover ? pIdx->zName : ""
    );
  }
}