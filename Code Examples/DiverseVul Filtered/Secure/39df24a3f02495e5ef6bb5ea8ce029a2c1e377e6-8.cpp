static int tableAndColumnIndex(
  SrcList *pSrc,       /* Array of tables to search */
  int N,               /* Number of tables in pSrc->a[] to search */
  const char *zCol,    /* Name of the column we are looking for */
  int *piTab,          /* Write index of pSrc->a[] here */
  int *piCol,          /* Write index of pSrc->a[*piTab].pTab->aCol[] here */
  int bIgnoreHidden    /* True to ignore hidden columns */
){
  int i;               /* For looping over tables in pSrc */
  int iCol;            /* Index of column matching zCol */

  assert( (piTab==0)==(piCol==0) );  /* Both or neither are NULL */
  for(i=0; i<N; i++){
    iCol = columnIndex(pSrc->a[i].pTab, zCol);
    if( iCol>=0 
     && (bIgnoreHidden==0 || IsHiddenColumn(&pSrc->a[i].pTab->aCol[iCol])==0)
    ){
      if( piTab ){
        *piTab = i;
        *piCol = iCol;
      }
      return 1;
    }
  }
  return 0;
}