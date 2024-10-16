JsLex *jslSetLex(JsLex *l) {
  JsLex *old = lex;
  lex = l;
  return old;
}