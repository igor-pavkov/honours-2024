static void err_raise_ext(int errcode)
{
  static_uex.exclass = LJ_UEXCLASS_MAKE(errcode);
  static_uex.excleanup = NULL;
  _Unwind_RaiseException(&static_uex);
}