void sqlite3SelectWrongNumTermsError(Parse *pParse, Select *p){
  if( p->selFlags & SF_Values ){
    sqlite3ErrorMsg(pParse, "all VALUES must have the same number of terms");
  }else{
    sqlite3ErrorMsg(pParse, "SELECTs to the left and right of %s"
      " do not have the same number of result columns", selectOpName(p->op));
  }
}