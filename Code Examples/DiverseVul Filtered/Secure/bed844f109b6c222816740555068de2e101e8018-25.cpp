bool jslMatch(int expected_tk) {
  if (lex->tk != expected_tk) {
    jslMatchError(expected_tk);
    return false;
  }
  jslGetNextToken();
  return true;
}