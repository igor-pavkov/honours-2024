void load_config(void)
{
  switch (su_mode) {
  case SU_MODE:
    logindefs_load_file(_PATH_LOGINDEFS_SU);
    break;
  case RUNUSER_MODE:
    logindefs_load_file(_PATH_LOGINDEFS_RUNUSER);
    break;
  }

  logindefs_load_file(_PATH_LOGINDEFS);
}