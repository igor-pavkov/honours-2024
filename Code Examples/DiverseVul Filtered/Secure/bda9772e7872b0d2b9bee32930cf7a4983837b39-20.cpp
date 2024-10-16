bool TokLinuxStat(const struct stat *input, struct klinux_stat *output) {
  if (!input || !output) return false;
  output->klinux_st_atime = input->st_atime;
  output->klinux_st_blksize = input->st_blksize;
  output->klinux_st_blocks = input->st_blocks;
  output->klinux_st_mtime = input->st_mtime;
  output->klinux_st_dev = input->st_dev;
  output->klinux_st_gid = input->st_gid;
  output->klinux_st_ino = input->st_ino;
  output->klinux_st_mode = input->st_mode;
  output->klinux_st_ctime = input->st_ctime;
  output->klinux_st_nlink = input->st_nlink;
  output->klinux_st_rdev = input->st_rdev;
  output->klinux_st_size = input->st_size;
  output->klinux_st_uid = input->st_uid;
  return true;
}