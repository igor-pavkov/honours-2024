LJ_FUNCA int lj_err_unwind_arm(int state, void *ucb, _Unwind_Context *ctx)
{
  void *cf = (void *)_Unwind_GetGR(ctx, 13);
  lua_State *L = cframe_L(cf);
  if ((state & _US_ACTION_MASK) == _US_VIRTUAL_UNWIND_FRAME) {
    setstrV(L, L->top++, lj_err_str(L, LJ_ERR_ERRCPP));
    return _URC_HANDLER_FOUND;
  }
  if ((state&(_US_ACTION_MASK|_US_FORCE_UNWIND)) == _US_UNWIND_FRAME_STARTING) {
    _Unwind_DeleteException(ucb);
    _Unwind_SetGR(ctx, 15, (uint32_t)(void *)lj_err_throw);
    _Unwind_SetGR(ctx, 0, (uint32_t)L);
    _Unwind_SetGR(ctx, 1, (uint32_t)LUA_ERRRUN);
    return _URC_INSTALL_CONTEXT;
  }
  if (__gnu_unwind_frame(ucb, ctx) != _URC_OK)
    return _URC_FAILURE;
  return _URC_CONTINUE_UNWIND;
}