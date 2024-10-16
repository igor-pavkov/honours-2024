LJ_NOINLINE void lj_err_callermsg(lua_State *L, const char *msg)
{
  TValue *frame = L->base-1;
  TValue *pframe = NULL;
  if (frame_islua(frame)) {
    pframe = frame_prevl(frame);
  } else if (frame_iscont(frame)) {
#if LJ_HASFFI
    if ((frame-1)->u32.lo == LJ_CONT_FFI_CALLBACK) {
      pframe = frame;
      frame = NULL;
    } else
#endif
    {
      pframe = frame_prevd(frame);
#if LJ_HASFFI
      /* Remove frame for FFI metamethods. */
      if (frame_func(frame)->c.ffid >= FF_ffi_meta___index &&
	  frame_func(frame)->c.ffid <= FF_ffi_meta___tostring) {
	L->base = pframe+1;
	L->top = frame;
	setcframe_pc(cframe_raw(L->cframe), frame_contpc(frame));
      }
#endif
    }
  }
  lj_debug_addloc(L, msg, pframe, frame);
  lj_err_run(L);
}