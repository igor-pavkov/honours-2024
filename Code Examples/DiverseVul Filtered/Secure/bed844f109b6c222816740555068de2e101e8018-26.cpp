static void jslMatchError(int expected_tk) {
  char gotStr[30];
  char expStr[30];
  jslGetTokenString(gotStr, sizeof(gotStr));
  jslTokenAsString(expected_tk, expStr, sizeof(expStr));

  size_t oldPos = lex->tokenLastStart;
  lex->tokenLastStart = jsvStringIteratorGetIndex(&lex->tokenStart.it)-1;
  jsExceptionHere(JSET_SYNTAXERROR, "Got %s expected %s", gotStr, expStr);
  lex->tokenLastStart = oldPos;
  // Sod it, skip this token anyway - stops us looping
  jslGetNextToken();
}