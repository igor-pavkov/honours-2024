LJ_NOINLINE void lj_err_lex(lua_State *L, GCstr *src, const char *tok,
			    BCLine line, ErrMsg em, va_list argp)
{
  char buff[LUA_IDSIZE];
  const char *msg;
  lj_debug_shortname(buff, src);
  msg = lj_str_pushvf(L, err2msg(em), argp);
  msg = lj_str_pushf(L, "%s:%d: %s", buff, line, msg);
  if (tok)
    lj_str_pushf(L, err2msg(LJ_ERR_XNEAR), msg, tok);
  lj_err_throw(L, LUA_ERRSYNTAX);
}