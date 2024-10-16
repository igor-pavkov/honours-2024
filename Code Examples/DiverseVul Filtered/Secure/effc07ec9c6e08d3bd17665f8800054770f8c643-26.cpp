static SQLITE_NOINLINE WhereTerm *whereScanInitIndexExpr(WhereScan *pScan){
  pScan->idxaff = sqlite3ExprAffinity(pScan->pIdxExpr);
  return whereScanNext(pScan);
}