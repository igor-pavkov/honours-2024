void jslGetNextToken() {
  jslGetNextToken_start:
  // Skip whitespace
  while (isWhitespace(lex->currCh))
    jslGetNextCh();
  // Search for comments
  if (lex->currCh=='/') {
    // newline comments
    if (jslNextCh()=='/') {
      while (lex->currCh && lex->currCh!='\n') jslGetNextCh();
      jslGetNextCh();
      goto jslGetNextToken_start;
    }
    // block comments
    if (jslNextCh()=='*') {
      jslGetNextCh();
      jslGetNextCh();
      while (lex->currCh && !(lex->currCh=='*' && jslNextCh()=='/'))
        jslGetNextCh();
      if (!lex->currCh) {
        lex->tk = LEX_UNFINISHED_COMMENT;
        return; /* an unfinished multi-line comment. When in interactive console,
                   detect this and make sure we accept new lines */
      }
      jslGetNextCh();
      jslGetNextCh();
      goto jslGetNextToken_start;
    }
  }
  int lastToken = lex->tk;
  lex->tk = LEX_EOF;
  lex->tokenl = 0; // clear token string
  if (lex->tokenValue) {
    jsvUnLock(lex->tokenValue);
    lex->tokenValue = 0;
  }
  // record beginning of this token
  lex->tokenLastStart = jsvStringIteratorGetIndex(&lex->tokenStart.it) - 1;
  /* we don't lock here, because we know that the string itself will be locked
   * because of lex->sourceVar */
  lex->tokenStart.it = lex->it;
  lex->tokenStart.currCh = lex->currCh;
  // tokens
  if (((unsigned char)lex->currCh) < jslJumpTableStart ||
      ((unsigned char)lex->currCh) > jslJumpTableEnd) {
    // if unhandled by the jump table, just pass it through as a single character
    jslSingleChar();
  } else {
    switch(jslJumpTable[((unsigned char)lex->currCh) - jslJumpTableStart]) {
    case JSLJT_ID: {
      while (isAlpha(lex->currCh) || isNumeric(lex->currCh) || lex->currCh=='$') {
        jslTokenAppendChar(lex->currCh);
        jslGetNextCh();
      }
      lex->tk = LEX_ID;
      // We do fancy stuff here to reduce number of compares (hopefully GCC creates a jump table)
      switch (lex->token[0]) {
      case 'b': if (jslIsToken("break", 1)) lex->tk = LEX_R_BREAK;
      break;
      case 'c': if (jslIsToken("case", 1)) lex->tk = LEX_R_CASE;
      else if (jslIsToken("catch", 1)) lex->tk = LEX_R_CATCH;
      else if (jslIsToken("class", 1)) lex->tk = LEX_R_CLASS;
      else if (jslIsToken("const", 1)) lex->tk = LEX_R_CONST;
      else if (jslIsToken("continue", 1)) lex->tk = LEX_R_CONTINUE;
      break;
      case 'd': if (jslIsToken("default", 1)) lex->tk = LEX_R_DEFAULT;
      else if (jslIsToken("delete", 1)) lex->tk = LEX_R_DELETE;
      else if (jslIsToken("do", 1)) lex->tk = LEX_R_DO;
      else if (jslIsToken("debugger", 1)) lex->tk = LEX_R_DEBUGGER;
      break;
      case 'e': if (jslIsToken("else", 1)) lex->tk = LEX_R_ELSE;
      else if (jslIsToken("extends", 1)) lex->tk = LEX_R_EXTENDS;
      break;
      case 'f': if (jslIsToken("false", 1)) lex->tk = LEX_R_FALSE;
      else if (jslIsToken("finally", 1)) lex->tk = LEX_R_FINALLY;
      else if (jslIsToken("for", 1)) lex->tk = LEX_R_FOR;
      else if (jslIsToken("function", 1)) lex->tk = LEX_R_FUNCTION;
      break;
      case 'i': if (jslIsToken("if", 1)) lex->tk = LEX_R_IF;
      else if (jslIsToken("in", 1)) lex->tk = LEX_R_IN;
      else if (jslIsToken("instanceof", 1)) lex->tk = LEX_R_INSTANCEOF;
      break;
      case 'l': if (jslIsToken("let", 1)) lex->tk = LEX_R_LET;
      break;
      case 'n': if (jslIsToken("new", 1)) lex->tk = LEX_R_NEW;
      else if (jslIsToken("null", 1)) lex->tk = LEX_R_NULL;
      break;
      case 'r': if (jslIsToken("return", 1)) lex->tk = LEX_R_RETURN;
      break;
      case 's': if (jslIsToken("static", 1)) lex->tk = LEX_R_STATIC;
      else if (jslIsToken("super", 1)) lex->tk = LEX_R_SUPER;
      else if (jslIsToken("switch", 1)) lex->tk = LEX_R_SWITCH;
      break;
      case 't': if (jslIsToken("this", 1)) lex->tk = LEX_R_THIS;
      else if (jslIsToken("throw", 1)) lex->tk = LEX_R_THROW;
      else if (jslIsToken("true", 1)) lex->tk = LEX_R_TRUE;
      else if (jslIsToken("try", 1)) lex->tk = LEX_R_TRY;
      else if (jslIsToken("typeof", 1)) lex->tk = LEX_R_TYPEOF;
      break;
      case 'u': if (jslIsToken("undefined", 1)) lex->tk = LEX_R_UNDEFINED;
      break;
      case 'w': if (jslIsToken("while", 1)) lex->tk = LEX_R_WHILE;
      break;
      case 'v': if (jslIsToken("var", 1)) lex->tk = LEX_R_VAR;
      else if (jslIsToken("void", 1)) lex->tk = LEX_R_VOID;
      break;
      default: break;
      } break;
      case JSLJT_NUMBER: {
        // TODO: check numbers aren't the wrong format
        bool canBeFloating = true;
        if (lex->currCh=='.') {
          jslGetNextCh();
          if (isNumeric(lex->currCh)) {
            // it is a float
            lex->tk = LEX_FLOAT;
            jslTokenAppendChar('.');
          } else {
            // it wasn't a number after all
            lex->tk = '.';
            break;
          }
        } else {
          if (lex->currCh=='0') {
            jslTokenAppendChar(lex->currCh);
            jslGetNextCh();
            if ((lex->currCh=='x' || lex->currCh=='X') ||
                (lex->currCh=='b' || lex->currCh=='B') ||
                (lex->currCh=='o' || lex->currCh=='O')) {
              canBeFloating = false;
              jslTokenAppendChar(lex->currCh); jslGetNextCh();
            }
          }
          lex->tk = LEX_INT;
          while (isNumeric(lex->currCh) || (!canBeFloating && isHexadecimal(lex->currCh))) {
            jslTokenAppendChar(lex->currCh);
            jslGetNextCh();
          }
          if (canBeFloating && lex->currCh=='.') {
            lex->tk = LEX_FLOAT;
            jslTokenAppendChar('.');
            jslGetNextCh();
          }
        }
        // parse fractional part
        if (lex->tk == LEX_FLOAT) {
          while (isNumeric(lex->currCh)) {
            jslTokenAppendChar(lex->currCh);
            jslGetNextCh();
          }
        }
        // do fancy e-style floating point
        if (canBeFloating && (lex->currCh=='e'||lex->currCh=='E')) {
          lex->tk = LEX_FLOAT;
          jslTokenAppendChar(lex->currCh); jslGetNextCh();
          if (lex->currCh=='-' || lex->currCh=='+') { jslTokenAppendChar(lex->currCh); jslGetNextCh(); }
          while (isNumeric(lex->currCh)) {
            jslTokenAppendChar(lex->currCh); jslGetNextCh();
          }
        }
      } break;
      case JSLJT_STRING: jslLexString(); break;
      case JSLJT_EXCLAMATION: jslSingleChar();
      if (lex->currCh=='=') { // !=
        lex->tk = LEX_NEQUAL;
        jslGetNextCh();
        if (lex->currCh=='=') { // !==
          lex->tk = LEX_NTYPEEQUAL;
          jslGetNextCh();
        }
      } break;
      case JSLJT_PLUS: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_PLUSEQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='+') {
        lex->tk = LEX_PLUSPLUS;
        jslGetNextCh();
      } break;
      case JSLJT_MINUS: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_MINUSEQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='-') {
        lex->tk = LEX_MINUSMINUS;
        jslGetNextCh();
      } break;
      case JSLJT_AND: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_ANDEQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='&') {
        lex->tk = LEX_ANDAND;
        jslGetNextCh();
      } break;
      case JSLJT_OR: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_OREQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='|') {
        lex->tk = LEX_OROR;
        jslGetNextCh();
      } break;
      case JSLJT_TOPHAT: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_XOREQUAL;
        jslGetNextCh();
      } break;
      case JSLJT_STAR: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_MULEQUAL;
        jslGetNextCh();
      } break;
      case JSLJT_FORWARDSLASH:
      // yay! JS is so awesome.
      if (lastToken==LEX_EOF ||
          lastToken=='!' ||
          lastToken=='%' ||
          lastToken=='&' ||
          lastToken=='*' ||
          lastToken=='+' ||
          lastToken=='-' ||
          lastToken=='/' ||
          lastToken=='<' ||
          lastToken=='=' ||
          lastToken=='>' ||
          lastToken=='?' ||
          (lastToken>=_LEX_OPERATOR_START && lastToken<=_LEX_OPERATOR_END) ||
          (lastToken>=_LEX_R_LIST_START && lastToken<=_LEX_R_LIST_END) || // keywords
          lastToken==LEX_R_CASE ||
          lastToken==LEX_R_NEW ||
          lastToken=='[' ||
          lastToken=='{' ||
          lastToken=='}' ||
          lastToken=='(' ||
          lastToken==',' ||
          lastToken==';' ||
          lastToken==':' ||
          lastToken==LEX_ARROW_FUNCTION) {
        // EOF operator keyword case new [ { } ( , ; : =>
        // phew. We're a regex
        jslLexRegex();
      } else {
        jslSingleChar();
        if (lex->currCh=='=') {
          lex->tk = LEX_DIVEQUAL;
          jslGetNextCh();
        }
      } break;
      case JSLJT_PERCENT: jslSingleChar();
      if (lex->currCh=='=') {
        lex->tk = LEX_MODEQUAL;
        jslGetNextCh();
      } break;
      case JSLJT_EQUAL: jslSingleChar();
      if (lex->currCh=='=') { // ==
        lex->tk = LEX_EQUAL;
        jslGetNextCh();
        if (lex->currCh=='=') { // ===
          lex->tk = LEX_TYPEEQUAL;
          jslGetNextCh();
        }
      } else if (lex->currCh=='>') { // =>
        lex->tk = LEX_ARROW_FUNCTION;
        jslGetNextCh();
      } break;
      case JSLJT_LESSTHAN: jslSingleChar();
      if (lex->currCh=='=') { // <=
        lex->tk = LEX_LEQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='<') { // <<
        lex->tk = LEX_LSHIFT;
        jslGetNextCh();
        if (lex->currCh=='=') { // <<=
          lex->tk = LEX_LSHIFTEQUAL;
          jslGetNextCh();
        }
      } break;
      case JSLJT_GREATERTHAN: jslSingleChar();
      if (lex->currCh=='=') { // >=
        lex->tk = LEX_GEQUAL;
        jslGetNextCh();
      } else if (lex->currCh=='>') { // >>
        lex->tk = LEX_RSHIFT;
        jslGetNextCh();
        if (lex->currCh=='=') { // >>=
          lex->tk = LEX_RSHIFTEQUAL;
          jslGetNextCh();
        } else if (lex->currCh=='>') { // >>>
          jslGetNextCh();
          if (lex->currCh=='=') { // >>>=
            lex->tk = LEX_RSHIFTUNSIGNEDEQUAL;
            jslGetNextCh();
          } else {
            lex->tk = LEX_RSHIFTUNSIGNED;
          }
        }
      } break;

      case JSLJT_SINGLECHAR: jslSingleChar(); break;
      default: assert(0);break;
    }
    }
  }
}