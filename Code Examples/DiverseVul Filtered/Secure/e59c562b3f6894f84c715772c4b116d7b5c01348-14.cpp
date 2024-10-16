static void sqlite3SelectAddTypeInfo(Parse *pParse, Select *pSelect){
#ifndef SQLITE_OMIT_SUBQUERY
  Walker w;
  w.xSelectCallback = sqlite3SelectWalkNoop;
  w.xSelectCallback2 = selectAddSubqueryTypeInfo;
  w.xExprCallback = sqlite3ExprWalkNoop;
  w.pParse = pParse;
  sqlite3WalkSelect(&w, pSelect);
#endif
}