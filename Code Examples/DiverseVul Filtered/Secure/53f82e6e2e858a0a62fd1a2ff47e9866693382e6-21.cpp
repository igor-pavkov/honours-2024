LJ_NOINLINE void LJ_FASTCALL lj_err_throw(lua_State *L, int errcode)
{
  global_State *g = G(L);
  lj_trace_abort(g);
  setgcrefnull(g->jit_L);
  L->status = 0;
#if LJ_UNWIND_EXT
  err_raise_ext(errcode);
  /*
  ** A return from this function signals a corrupt C stack that cannot be
  ** unwound. We have no choice but to call the panic function and exit.
  **
  ** Usually this is caused by a C function without unwind information.
  ** This should never happen on x64, but may happen if you've manually
  ** enabled LUAJIT_UNWIND_EXTERNAL and forgot to recompile *every*
  ** non-C++ file with -funwind-tables.
  */
  if (G(L)->panic)
    G(L)->panic(L);
#else
  {
    void *cf = err_unwind(L, NULL, errcode);
    if (cframe_unwind_ff(cf))
      lj_vm_unwind_ff(cframe_raw(cf));
    else
      lj_vm_unwind_c(cframe_raw(cf), errcode);
  }
#endif
  exit(EXIT_FAILURE);
}