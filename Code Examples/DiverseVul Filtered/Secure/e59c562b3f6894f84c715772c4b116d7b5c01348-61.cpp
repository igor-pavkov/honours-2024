int sqlite3JoinType(Parse *pParse, Token *pA, Token *pB, Token *pC){
  int jointype = 0;
  Token *apAll[3];
  Token *p;
                             /*   0123456789 123456789 123456789 123 */
  static const char zKeyText[] = "naturaleftouterightfullinnercross";
  static const struct {
    u8 i;        /* Beginning of keyword text in zKeyText[] */
    u8 nChar;    /* Length of the keyword in characters */
    u8 code;     /* Join type mask */
  } aKeyword[] = {
    /* natural */ { 0,  7, JT_NATURAL                },
    /* left    */ { 6,  4, JT_LEFT|JT_OUTER          },
    /* outer   */ { 10, 5, JT_OUTER                  },
    /* right   */ { 14, 5, JT_RIGHT|JT_OUTER         },
    /* full    */ { 19, 4, JT_LEFT|JT_RIGHT|JT_OUTER },
    /* inner   */ { 23, 5, JT_INNER                  },
    /* cross   */ { 28, 5, JT_INNER|JT_CROSS         },
  };
  int i, j;
  apAll[0] = pA;
  apAll[1] = pB;
  apAll[2] = pC;
  for(i=0; i<3 && apAll[i]; i++){
    p = apAll[i];
    for(j=0; j<ArraySize(aKeyword); j++){
      if( p->n==aKeyword[j].nChar 
          && sqlite3StrNICmp((char*)p->z, &zKeyText[aKeyword[j].i], p->n)==0 ){
        jointype |= aKeyword[j].code;
        break;
      }
    }
    testcase( j==0 || j==1 || j==2 || j==3 || j==4 || j==5 || j==6 );
    if( j>=ArraySize(aKeyword) ){
      jointype |= JT_ERROR;
      break;
    }
  }
  if(
     (jointype & (JT_INNER|JT_OUTER))==(JT_INNER|JT_OUTER) ||
     (jointype & JT_ERROR)!=0
  ){
    const char *zSp = " ";
    assert( pB!=0 );
    if( pC==0 ){ zSp++; }
    sqlite3ErrorMsg(pParse, "unknown or unsupported join type: "
       "%T %T%s%T", pA, pB, zSp, pC);
    jointype = JT_INNER;
  }else if( (jointype & JT_OUTER)!=0 
         && (jointype & (JT_LEFT|JT_RIGHT))!=JT_LEFT ){
    sqlite3ErrorMsg(pParse, 
      "RIGHT and FULL OUTER JOINs are not currently supported");
    jointype = JT_INNER;
  }
  return jointype;
}