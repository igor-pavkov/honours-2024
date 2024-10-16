bool PasswdHolderToPasswd(struct passwd_holder *passwd_in,
                          struct passwd *passwd_out) {
  if (!passwd_in || !passwd_out) {
    return false;
  }

  passwd_out->pw_name = passwd_in->pw_name;
  passwd_out->pw_passwd = passwd_in->pw_passwd;
  passwd_out->pw_uid = passwd_in->pw_uid;
  passwd_out->pw_gid = passwd_in->pw_gid;
  passwd_out->pw_gecos = passwd_in->pw_gecos;
  passwd_out->pw_dir = passwd_in->pw_dir;
  passwd_out->pw_shell = passwd_in->pw_shell;
  return true;
}