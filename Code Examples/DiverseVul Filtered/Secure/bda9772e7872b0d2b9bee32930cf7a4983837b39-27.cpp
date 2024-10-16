bool FromkLinuxStat(const struct klinux_stat *input, struct stat *output) {
  if (!input || !output) return false;
  output->st_atime = input->klinux_st_atime;
  output->st_blksize = input->klinux_st_blksize;
  output->st_blocks = input->klinux_st_blocks;
  output->st_mtime = input->klinux_st_mtime;
  output->st_dev = input->klinux_st_dev;
  output->st_gid = input->klinux_st_gid;
  output->st_ino = input->klinux_st_ino;
  output->st_mode = input->klinux_st_mode;
  output->st_ctime = input->klinux_st_ctime;
  output->st_nlink = input->klinux_st_nlink;
  output->st_rdev = input->klinux_st_rdev;
  output->st_size = input->klinux_st_size;
  output->st_uid = input->klinux_st_uid;
  return true;
}