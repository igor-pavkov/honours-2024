static void *err_unwind(lua_State *L, void *stopcf, int errcode)
{
  TValue *frame = L->base-1;
  void *cf = L->cframe;
  while (cf) {
    int32_t nres = cframe_nres(cframe_raw(cf));
    if (nres < 0) {  /* C frame without Lua frame? */
      TValue *top = restorestack(L, -nres);
      if (frame < top) {  /* Frame reached? */
	if (errcode) {
	  L->cframe = cframe_prev(cf);
	  L->base = frame+1;
	  unwindstack(L, top);
	}
	return cf;
      }
    }
    if (frame <= tvref(L->stack))
      break;
    switch (frame_typep(frame)) {
    case FRAME_LUA:  /* Lua frame. */
    case FRAME_LUAP:
      frame = frame_prevl(frame);
      break;
    case FRAME_C:  /* C frame. */
#if LJ_HASFFI
    unwind_c:
#endif
#if LJ_UNWIND_EXT
      if (errcode) {
	L->cframe = cframe_prev(cf);
	L->base = frame_prevd(frame) + 1;
	unwindstack(L, frame);
      } else if (cf != stopcf) {
	cf = cframe_prev(cf);
	frame = frame_prevd(frame);
	break;
      }
      return NULL;  /* Continue unwinding. */
#else
      UNUSED(stopcf);
      cf = cframe_prev(cf);
      frame = frame_prevd(frame);
      break;
#endif
    case FRAME_CP:  /* Protected C frame. */
      if (cframe_canyield(cf)) {  /* Resume? */
	if (errcode) {
	  hook_leave(G(L));  /* Assumes nobody uses coroutines inside hooks. */
	  L->cframe = NULL;
	  L->status = (uint8_t)errcode;
	}
	return cf;
      }
      if (errcode) {
	L->cframe = cframe_prev(cf);
	L->base = frame_prevd(frame) + 1;
	unwindstack(L, frame);
      }
      return cf;
    case FRAME_CONT:  /* Continuation frame. */
#if LJ_HASFFI
      if ((frame-1)->u32.lo == LJ_CONT_FFI_CALLBACK)
	goto unwind_c;
#endif
      /* fallthrough */
    case FRAME_VARG:  /* Vararg frame. */
      frame = frame_prevd(frame);
      break;
    case FRAME_PCALL:  /* FF pcall() frame. */
    case FRAME_PCALLH:  /* FF pcall() frame inside hook. */
      if (errcode) {
	if (errcode == LUA_YIELD) {
	  frame = frame_prevd(frame);
	  break;
	}
	if (frame_typep(frame) == FRAME_PCALL)
	  hook_leave(G(L));
	L->cframe = cf;
	L->base = frame_prevd(frame) + 1;
	unwindstack(L, L->base);
      }
      return (void *)((intptr_t)cf | CFRAME_UNWIND_FF);
    }
  }
  /* No C frame. */
  if (errcode) {
    L->cframe = NULL;
    L->base = tvref(L->stack)+1;
    unwindstack(L, L->base);
    if (G(L)->panic)
      G(L)->panic(L);
    exit(EXIT_FAILURE);
  }
  return L;  /* Anything non-NULL will do. */
}