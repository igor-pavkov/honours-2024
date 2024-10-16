UNKNOWN_ESC_WARN(ScanEnv *env, int c)
{
  if (onig_warn == onig_null_warn || !RTEST(ruby_verbose)) return ;
  onig_syntax_warn(env, "Unknown escape \\%c is ignored", c);
}