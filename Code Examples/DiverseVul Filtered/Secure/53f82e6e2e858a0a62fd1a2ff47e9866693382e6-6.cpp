LJ_NOINLINE void lj_err_mem(lua_State *L)
{
  if (L->status == LUA_ERRERR+1)  /* Don't touch the stack during lua_open. */
    lj_vm_unwind_c(L->cframe, LUA_ERRMEM);
  setstrV(L, L->top++, lj_err_str(L, LJ_ERR_ERRMEM));
  lj_err_throw(L, LUA_ERRMEM);
}