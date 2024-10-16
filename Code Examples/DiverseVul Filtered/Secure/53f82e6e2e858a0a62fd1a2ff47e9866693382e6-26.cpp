LJ_FUNCA EXCEPTION_DISPOSITION lj_err_unwind_win64(EXCEPTION_RECORD *rec,
  void *cf, CONTEXT *ctx, UndocumentedDispatcherContext *dispatch)
{
  lua_State *L = cframe_L(cf);
  int errcode = LJ_EXCODE_CHECK(rec->ExceptionCode) ?
		LJ_EXCODE_ERRCODE(rec->ExceptionCode) : LUA_ERRRUN;
  if ((rec->ExceptionFlags & 6)) {  /* EH_UNWINDING|EH_EXIT_UNWIND */
    /* Unwind internal frames. */
    err_unwind(L, cf, errcode);
  } else {
    void *cf2 = err_unwind(L, cf, 0);
    if (cf2) {  /* We catch it, so start unwinding the upper frames. */
      if (rec->ExceptionCode == LJ_MSVC_EXCODE ||
	  rec->ExceptionCode == LJ_GCC_EXCODE) {
#if LJ_TARGET_WINDOWS
	__DestructExceptionObject(rec, 1);
#endif
	setstrV(L, L->top++, lj_err_str(L, LJ_ERR_ERRCPP));
      } else if (!LJ_EXCODE_CHECK(rec->ExceptionCode)) {
	/* Don't catch access violations etc. */
	return ExceptionContinueSearch;
      }
      /* Unwind the stack and call all handlers for all lower C frames
      ** (including ourselves) again with EH_UNWINDING set. Then set
      ** rsp = cf, rax = errcode and jump to the specified target.
      */
      RtlUnwindEx(cf, (void *)((cframe_unwind_ff(cf2) && errcode != LUA_YIELD) ?
			       lj_vm_unwind_ff_eh :
			       lj_vm_unwind_c_eh),
		  rec, (void *)(uintptr_t)errcode, ctx, dispatch->HistoryTable);
      /* RtlUnwindEx should never return. */
    }
  }
  return ExceptionContinueSearch;
}