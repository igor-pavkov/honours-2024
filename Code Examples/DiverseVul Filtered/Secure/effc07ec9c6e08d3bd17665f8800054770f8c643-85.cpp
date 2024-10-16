void sqlite3WhereEnd(WhereInfo *pWInfo){
  Parse *pParse = pWInfo->pParse;
  Vdbe *v = pParse->pVdbe;
  int i;
  WhereLevel *pLevel;
  WhereLoop *pLoop;
  SrcList *pTabList = pWInfo->pTabList;
  sqlite3 *db = pParse->db;
  int iEnd = sqlite3VdbeCurrentAddr(v);
  int nRJ = 0;

  /* Generate loop termination code.
  */
  VdbeModuleComment((v, "End WHERE-core"));
  for(i=pWInfo->nLevel-1; i>=0; i--){
    int addr;
    pLevel = &pWInfo->a[i];
    if( pLevel->pRJ ){
      /* Terminate the subroutine that forms the interior of the loop of
      ** the RIGHT JOIN table */
      WhereRightJoin *pRJ = pLevel->pRJ;
      sqlite3VdbeResolveLabel(v, pLevel->addrCont);
      pLevel->addrCont = 0;
      pRJ->endSubrtn = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp3(v, OP_Return, pRJ->regReturn, pRJ->addrSubrtn, 1);
      VdbeCoverage(v);
      nRJ++;
    }
    pLoop = pLevel->pWLoop;
    if( pLevel->op!=OP_Noop ){
#ifndef SQLITE_DISABLE_SKIPAHEAD_DISTINCT
      int addrSeek = 0;
      Index *pIdx;
      int n;
      if( pWInfo->eDistinct==WHERE_DISTINCT_ORDERED
       && i==pWInfo->nLevel-1  /* Ticket [ef9318757b152e3] 2017-10-21 */
       && (pLoop->wsFlags & WHERE_INDEXED)!=0
       && (pIdx = pLoop->u.btree.pIndex)->hasStat1
       && (n = pLoop->u.btree.nDistinctCol)>0
       && pIdx->aiRowLogEst[n]>=36
      ){
        int r1 = pParse->nMem+1;
        int j, op;
        for(j=0; j<n; j++){
          sqlite3VdbeAddOp3(v, OP_Column, pLevel->iIdxCur, j, r1+j);
        }
        pParse->nMem += n+1;
        op = pLevel->op==OP_Prev ? OP_SeekLT : OP_SeekGT;
        addrSeek = sqlite3VdbeAddOp4Int(v, op, pLevel->iIdxCur, 0, r1, n);
        VdbeCoverageIf(v, op==OP_SeekLT);
        VdbeCoverageIf(v, op==OP_SeekGT);
        sqlite3VdbeAddOp2(v, OP_Goto, 1, pLevel->p2);
      }
#endif /* SQLITE_DISABLE_SKIPAHEAD_DISTINCT */
      /* The common case: Advance to the next row */
      if( pLevel->addrCont ) sqlite3VdbeResolveLabel(v, pLevel->addrCont);
      sqlite3VdbeAddOp3(v, pLevel->op, pLevel->p1, pLevel->p2, pLevel->p3);
      sqlite3VdbeChangeP5(v, pLevel->p5);
      VdbeCoverage(v);
      VdbeCoverageIf(v, pLevel->op==OP_Next);
      VdbeCoverageIf(v, pLevel->op==OP_Prev);
      VdbeCoverageIf(v, pLevel->op==OP_VNext);
      if( pLevel->regBignull ){
        sqlite3VdbeResolveLabel(v, pLevel->addrBignull);
        sqlite3VdbeAddOp2(v, OP_DecrJumpZero, pLevel->regBignull, pLevel->p2-1);
        VdbeCoverage(v);
      }
#ifndef SQLITE_DISABLE_SKIPAHEAD_DISTINCT
      if( addrSeek ) sqlite3VdbeJumpHere(v, addrSeek);
#endif
    }else if( pLevel->addrCont ){
      sqlite3VdbeResolveLabel(v, pLevel->addrCont);
    }
    if( (pLoop->wsFlags & WHERE_IN_ABLE)!=0 && pLevel->u.in.nIn>0 ){
      struct InLoop *pIn;
      int j;
      sqlite3VdbeResolveLabel(v, pLevel->addrNxt);
      for(j=pLevel->u.in.nIn, pIn=&pLevel->u.in.aInLoop[j-1]; j>0; j--, pIn--){
        assert( sqlite3VdbeGetOp(v, pIn->addrInTop+1)->opcode==OP_IsNull
                 || pParse->db->mallocFailed );
        sqlite3VdbeJumpHere(v, pIn->addrInTop+1);
        if( pIn->eEndLoopOp!=OP_Noop ){
          if( pIn->nPrefix ){
            int bEarlyOut = 
                (pLoop->wsFlags & WHERE_VIRTUALTABLE)==0
                 && (pLoop->wsFlags & WHERE_IN_EARLYOUT)!=0;
            if( pLevel->iLeftJoin ){
              /* For LEFT JOIN queries, cursor pIn->iCur may not have been
              ** opened yet. This occurs for WHERE clauses such as
              ** "a = ? AND b IN (...)", where the index is on (a, b). If
              ** the RHS of the (a=?) is NULL, then the "b IN (...)" may
              ** never have been coded, but the body of the loop run to
              ** return the null-row. So, if the cursor is not open yet,
              ** jump over the OP_Next or OP_Prev instruction about to
              ** be coded.  */
              sqlite3VdbeAddOp2(v, OP_IfNotOpen, pIn->iCur, 
                  sqlite3VdbeCurrentAddr(v) + 2 + bEarlyOut);
              VdbeCoverage(v);
            }
            if( bEarlyOut ){
              sqlite3VdbeAddOp4Int(v, OP_IfNoHope, pLevel->iIdxCur,
                  sqlite3VdbeCurrentAddr(v)+2,
                  pIn->iBase, pIn->nPrefix);
              VdbeCoverage(v);
              /* Retarget the OP_IsNull against the left operand of IN so 
              ** it jumps past the OP_IfNoHope.  This is because the
              ** OP_IsNull also bypasses the OP_Affinity opcode that is
              ** required by OP_IfNoHope. */
              sqlite3VdbeJumpHere(v, pIn->addrInTop+1);
            }
          }
          sqlite3VdbeAddOp2(v, pIn->eEndLoopOp, pIn->iCur, pIn->addrInTop);
          VdbeCoverage(v);
          VdbeCoverageIf(v, pIn->eEndLoopOp==OP_Prev);
          VdbeCoverageIf(v, pIn->eEndLoopOp==OP_Next);
        }
        sqlite3VdbeJumpHere(v, pIn->addrInTop-1);
      }
    }
    sqlite3VdbeResolveLabel(v, pLevel->addrBrk);
    if( pLevel->pRJ ){
      sqlite3VdbeAddOp3(v, OP_Return, pLevel->pRJ->regReturn, 0, 1);
      VdbeCoverage(v);
    }
    if( pLevel->addrSkip ){
      sqlite3VdbeGoto(v, pLevel->addrSkip);
      VdbeComment((v, "next skip-scan on %s", pLoop->u.btree.pIndex->zName));
      sqlite3VdbeJumpHere(v, pLevel->addrSkip);
      sqlite3VdbeJumpHere(v, pLevel->addrSkip-2);
    }
#ifndef SQLITE_LIKE_DOESNT_MATCH_BLOBS
    if( pLevel->addrLikeRep ){
      sqlite3VdbeAddOp2(v, OP_DecrJumpZero, (int)(pLevel->iLikeRepCntr>>1),
                        pLevel->addrLikeRep);
      VdbeCoverage(v);
    }
#endif
    if( pLevel->iLeftJoin ){
      int ws = pLoop->wsFlags;
      addr = sqlite3VdbeAddOp1(v, OP_IfPos, pLevel->iLeftJoin); VdbeCoverage(v);
      assert( (ws & WHERE_IDX_ONLY)==0 || (ws & WHERE_INDEXED)!=0 );
      if( (ws & WHERE_IDX_ONLY)==0 ){
        assert( pLevel->iTabCur==pTabList->a[pLevel->iFrom].iCursor );
        sqlite3VdbeAddOp1(v, OP_NullRow, pLevel->iTabCur);
      }
      if( (ws & WHERE_INDEXED) 
       || ((ws & WHERE_MULTI_OR) && pLevel->u.pCoveringIdx) 
      ){
        if( ws & WHERE_MULTI_OR ){
          Index *pIx = pLevel->u.pCoveringIdx;
          int iDb = sqlite3SchemaToIndex(db, pIx->pSchema);
          sqlite3VdbeAddOp3(v, OP_ReopenIdx, pLevel->iIdxCur, pIx->tnum, iDb);
          sqlite3VdbeSetP4KeyInfo(pParse, pIx);
        }
        sqlite3VdbeAddOp1(v, OP_NullRow, pLevel->iIdxCur);
      }
      if( pLevel->op==OP_Return ){
        sqlite3VdbeAddOp2(v, OP_Gosub, pLevel->p1, pLevel->addrFirst);
      }else{
        sqlite3VdbeGoto(v, pLevel->addrFirst);
      }
      sqlite3VdbeJumpHere(v, addr);
    }
    VdbeModuleComment((v, "End WHERE-loop%d: %s", i,
                     pWInfo->pTabList->a[pLevel->iFrom].pTab->zName));
  }

  assert( pWInfo->nLevel<=pTabList->nSrc );
  if( pWInfo->pExprMods ) whereUndoExprMods(pWInfo);
  for(i=0, pLevel=pWInfo->a; i<pWInfo->nLevel; i++, pLevel++){
    int k, last;
    VdbeOp *pOp, *pLastOp;
    Index *pIdx = 0;
    SrcItem *pTabItem = &pTabList->a[pLevel->iFrom];
    Table *pTab = pTabItem->pTab;
    assert( pTab!=0 );
    pLoop = pLevel->pWLoop;

    /* Do RIGHT JOIN processing.  Generate code that will output the
    ** unmatched rows of the right operand of the RIGHT JOIN with
    ** all of the columns of the left operand set to NULL.
    */
    if( pLevel->pRJ ){
      sqlite3WhereRightJoinLoop(pWInfo, i, pLevel);
      continue;
    }

    /* For a co-routine, change all OP_Column references to the table of
    ** the co-routine into OP_Copy of result contained in a register.
    ** OP_Rowid becomes OP_Null.
    */
    if( pTabItem->fg.viaCoroutine ){
      testcase( pParse->db->mallocFailed );
      translateColumnToCopy(pParse, pLevel->addrBody, pLevel->iTabCur,
                            pTabItem->regResult, 0);
      continue;
    }

    /* If this scan uses an index, make VDBE code substitutions to read data
    ** from the index instead of from the table where possible.  In some cases
    ** this optimization prevents the table from ever being read, which can
    ** yield a significant performance boost.
    ** 
    ** Calls to the code generator in between sqlite3WhereBegin and
    ** sqlite3WhereEnd will have created code that references the table
    ** directly.  This loop scans all that code looking for opcodes
    ** that reference the table and converts them into opcodes that
    ** reference the index.
    */
    if( pLoop->wsFlags & (WHERE_INDEXED|WHERE_IDX_ONLY) ){
      pIdx = pLoop->u.btree.pIndex;
    }else if( pLoop->wsFlags & WHERE_MULTI_OR ){
      pIdx = pLevel->u.pCoveringIdx;
    }
    if( pIdx
     && !db->mallocFailed
    ){
      if( pWInfo->eOnePass==ONEPASS_OFF || !HasRowid(pIdx->pTable) ){
        last = iEnd;
      }else{
        last = pWInfo->iEndWhere;
      }
      k = pLevel->addrBody + 1;
#ifdef SQLITE_DEBUG
      if( db->flags & SQLITE_VdbeAddopTrace ){
        printf("TRANSLATE opcodes in range %d..%d\n", k, last-1);
      }
      /* Proof that the "+1" on the k value above is safe */
      pOp = sqlite3VdbeGetOp(v, k - 1);
      assert( pOp->opcode!=OP_Column || pOp->p1!=pLevel->iTabCur );
      assert( pOp->opcode!=OP_Rowid  || pOp->p1!=pLevel->iTabCur );
      assert( pOp->opcode!=OP_IfNullRow || pOp->p1!=pLevel->iTabCur );
#endif
      pOp = sqlite3VdbeGetOp(v, k);
      pLastOp = pOp + (last - k);
      assert( pOp<=pLastOp );
      do{
        if( pOp->p1!=pLevel->iTabCur ){
          /* no-op */
        }else if( pOp->opcode==OP_Column
#ifdef SQLITE_ENABLE_OFFSET_SQL_FUNC
         || pOp->opcode==OP_Offset
#endif
        ){
          int x = pOp->p2;
          assert( pIdx->pTable==pTab );
#ifdef SQLITE_ENABLE_OFFSET_SQL_FUNC
          if( pOp->opcode==OP_Offset ){
            /* Do not need to translate the column number */
          }else
#endif
          if( !HasRowid(pTab) ){
            Index *pPk = sqlite3PrimaryKeyIndex(pTab);
            x = pPk->aiColumn[x];
            assert( x>=0 );
          }else{
            testcase( x!=sqlite3StorageColumnToTable(pTab,x) );
            x = sqlite3StorageColumnToTable(pTab,x);
          }
          x = sqlite3TableColumnToIndex(pIdx, x);
          if( x>=0 ){
            pOp->p2 = x;
            pOp->p1 = pLevel->iIdxCur;
            OpcodeRewriteTrace(db, k, pOp);
          }else{
            /* Unable to translate the table reference into an index
            ** reference.  Verify that this is harmless - that the
            ** table being referenced really is open.
            */
#ifdef SQLITE_ENABLE_OFFSET_SQL_FUNC
            assert( (pLoop->wsFlags & WHERE_IDX_ONLY)==0
                 || cursorIsOpen(v,pOp->p1,k)
                 || pOp->opcode==OP_Offset
            );
#else
            assert( (pLoop->wsFlags & WHERE_IDX_ONLY)==0
                 || cursorIsOpen(v,pOp->p1,k)
            );
#endif
          }
        }else if( pOp->opcode==OP_Rowid ){
          pOp->p1 = pLevel->iIdxCur;
          pOp->opcode = OP_IdxRowid;
          OpcodeRewriteTrace(db, k, pOp);
        }else if( pOp->opcode==OP_IfNullRow ){
          pOp->p1 = pLevel->iIdxCur;
          OpcodeRewriteTrace(db, k, pOp);
        }
#ifdef SQLITE_DEBUG
        k++;
#endif
      }while( (++pOp)<pLastOp );
#ifdef SQLITE_DEBUG
      if( db->flags & SQLITE_VdbeAddopTrace ) printf("TRANSLATE complete\n");
#endif
    }
  }

  /* The "break" point is here, just past the end of the outer loop.
  ** Set it.
  */
  sqlite3VdbeResolveLabel(v, pWInfo->iBreak);

  /* Final cleanup
  */
  pParse->nQueryLoop = pWInfo->savedNQueryLoop;
  whereInfoFree(db, pWInfo);
  pParse->withinRJSubrtn -= nRJ;
  return;
}