static void err_raise_ext(int errcode)
{
  RaiseException(LJ_EXCODE_MAKE(errcode), 1 /* EH_NONCONTINUABLE */, 0, NULL);
}