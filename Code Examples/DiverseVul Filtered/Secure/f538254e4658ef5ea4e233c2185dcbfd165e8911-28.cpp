void base_context::set_acceleration_functions(enum de265_acceleration l)
{
  // fill scalar functions first (so that function table is completely filled)

  init_acceleration_functions_fallback(&acceleration);


  // override functions with optimized variants

#ifdef HAVE_SSE4_1
  if (l>=de265_acceleration_SSE) {
    init_acceleration_functions_sse(&acceleration);
  }
#endif
#ifdef HAVE_ARM
  if (l>=de265_acceleration_ARM) {
    init_acceleration_functions_arm(&acceleration);
  }
#endif
}