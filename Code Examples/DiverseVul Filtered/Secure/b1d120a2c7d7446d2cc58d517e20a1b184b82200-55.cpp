bool DeserializePasswd(MessageReader *reader,
                       struct passwd_holder *passwd_buffers) {
  if (!reader || !passwd_buffers) {
    return false;
  }
  if (reader->size() < 7) {
    return false;
  }

  auto pw_name_buf = reader->next();
  auto pw_passwd_buf = reader->next();
  auto pw_uid = reader->next<uid_t>();
  auto pw_gid = reader->next<gid_t>();
  auto pw_gecos_buf = reader->next();
  auto pw_dir_buf = reader->next();
  auto pw_shell_buf = reader->next();

  strncpy(passwd_buffers->pw_name, pw_name_buf.As<char>(),
          std::min(sizeof(passwd_buffers->pw_name), pw_name_buf.size()));
  strncpy(passwd_buffers->pw_passwd, pw_passwd_buf.As<char>(),
          std::min(sizeof(passwd_buffers->pw_passwd), pw_passwd_buf.size()));
  passwd_buffers->pw_uid = pw_uid;
  passwd_buffers->pw_gid = pw_gid;
  strncpy(passwd_buffers->pw_gecos, pw_gecos_buf.As<char>(),
          std::min(sizeof(passwd_buffers->pw_gecos), pw_gecos_buf.size()));
  strncpy(passwd_buffers->pw_dir, pw_dir_buf.As<char>(),
          std::min(sizeof(passwd_buffers->pw_dir), pw_dir_buf.size()));
  strncpy(passwd_buffers->pw_shell, pw_shell_buf.As<char>(),
          std::min(sizeof(passwd_buffers->pw_shell), pw_shell_buf.size()));
  return true;
}