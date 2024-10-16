static void jslLexRegex() {
  lex->tokenValue = jsvNewFromEmptyString();
  if (!lex->tokenValue) {
    lex->tk = LEX_EOF;
    return;
  }
  JsvStringIterator it;
  jsvStringIteratorNew(&it, lex->tokenValue, 0);
  jsvStringIteratorAppend(&it, '/');
  // strings...
  jslGetNextCh();
  while (lex->currCh && lex->currCh!='/') {
    if (lex->currCh == '\\') {
      jsvStringIteratorAppend(&it, lex->currCh);
      jslGetNextCh();
    } else if (lex->currCh=='\n') {
      /* Was a newline - this is now allowed
       * unless we're a template string */
      break;
    }
    jsvStringIteratorAppend(&it, lex->currCh);
    jslGetNextCh();
  }
  lex->tk = LEX_REGEX;
  if (lex->currCh!='/') {
    lex->tk++; // +1 gets you to 'unfinished X'
  } else {
    jsvStringIteratorAppend(&it, '/');
    jslGetNextCh();
    // regex modifiers
    while (lex->currCh=='g' ||
        lex->currCh=='i' ||
        lex->currCh=='m' ||
        lex->currCh=='y' ||
        lex->currCh=='u') {
      jslTokenAppendChar(lex->currCh);
      jsvStringIteratorAppend(&it, lex->currCh);
      jslGetNextCh();
    }
  }
  jsvStringIteratorFree(&it);
}