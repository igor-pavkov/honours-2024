static int zipfileColumn(
  sqlite3_vtab_cursor *cur,   /* The cursor */
  sqlite3_context *ctx,       /* First argument to sqlite3_result_...() */
  int i                       /* Which column to return */
){
  ZipfileCsr *pCsr = (ZipfileCsr*)cur;
  ZipfileCDS *pCDS = &pCsr->pCurrent->cds;
  int rc = SQLITE_OK;
  switch( i ){
    case 0:   /* name */
      sqlite3_result_text(ctx, pCDS->zFile, -1, SQLITE_TRANSIENT);
      break;
    case 1:   /* mode */
      /* TODO: Whether or not the following is correct surely depends on
      ** the platform on which the archive was created.  */
      sqlite3_result_int(ctx, pCDS->iExternalAttr >> 16);
      break;
    case 2: { /* mtime */
      sqlite3_result_int64(ctx, pCsr->pCurrent->mUnixTime);
      break;
    }
    case 3: { /* sz */
      if( sqlite3_vtab_nochange(ctx)==0 ){
        sqlite3_result_int64(ctx, pCDS->szUncompressed);
      }
      break;
    }
    case 4:   /* rawdata */
      if( sqlite3_vtab_nochange(ctx) ) break;
    case 5: { /* data */
      if( i==4 || pCDS->iCompression==0 || pCDS->iCompression==8 ){
        int sz = pCDS->szCompressed;
        int szFinal = pCDS->szUncompressed;
        if( szFinal>0 ){
          u8 *aBuf;
          u8 *aFree = 0;
          if( pCsr->pCurrent->aData ){
            aBuf = pCsr->pCurrent->aData;
          }else{
            aBuf = aFree = sqlite3_malloc64(sz);
            if( aBuf==0 ){
              rc = SQLITE_NOMEM;
            }else{
              FILE *pFile = pCsr->pFile;
              if( pFile==0 ){
                pFile = ((ZipfileTab*)(pCsr->base.pVtab))->pWriteFd;
              }
              rc = zipfileReadData(pFile, aBuf, sz, pCsr->pCurrent->iDataOff,
                  &pCsr->base.pVtab->zErrMsg
              );
            }
          }
          if( rc==SQLITE_OK ){
            if( i==5 && pCDS->iCompression ){
              zipfileInflate(ctx, aBuf, sz, szFinal);
            }else{
              sqlite3_result_blob(ctx, aBuf, sz, SQLITE_TRANSIENT);
            }
          }
          sqlite3_free(aFree);
        }else{
          /* Figure out if this is a directory or a zero-sized file. Consider
          ** it to be a directory either if the mode suggests so, or if
          ** the final character in the name is '/'.  */
          u32 mode = pCDS->iExternalAttr >> 16;
          if( !(mode & S_IFDIR) && pCDS->zFile[pCDS->nFile-1]!='/' ){
            sqlite3_result_blob(ctx, "", 0, SQLITE_STATIC);
          }
        }
      }
      break;
    }
    case 6:   /* method */
      sqlite3_result_int(ctx, pCDS->iCompression);
      break;
    default:  /* z */
      assert( i==7 );
      sqlite3_result_int64(ctx, pCsr->iId);
      break;
  }

  return rc;
}