bool FromkLinuxStatFs(const struct klinux_statfs *input,
                      struct statfs *output) {
  if (!input || !output) return false;
  output->f_type = input->klinux_f_type;
  output->f_bsize = input->klinux_f_bsize;
  output->f_blocks = input->klinux_f_blocks;
  output->f_bfree = input->klinux_f_bfree;
  output->f_bavail = input->klinux_f_bavail;
  output->f_files = input->klinux_f_files;
  output->f_ffree = input->klinux_f_ffree;
  output->f_fsid.__val[0] = input->klinux_f_fsid.__val[0];
  output->f_fsid.__val[1] = input->klinux_f_fsid.__val[1];
  output->f_namelen = input->klinux_f_namelen;
  output->f_frsize = input->klinux_f_frsize;
  output->f_flags = input->klinux_f_flags;
  memset(output->f_spare, 0, sizeof(output->f_spare));
  return true;
}