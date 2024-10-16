bool TokLinuxStatFs(const struct statfs *input, struct klinux_statfs *output) {
  if (!input || !output) return false;
  output->klinux_f_bsize = input->f_bsize;
  output->klinux_f_frsize = input->f_frsize;
  output->klinux_f_blocks = input->f_blocks;
  output->klinux_f_bfree = input->f_bfree;
  output->klinux_f_bavail = input->f_bavail;
  output->klinux_f_files = input->f_files;
  output->klinux_f_ffree = input->f_ffree;
  output->klinux_f_fsid.__val[0] = input->f_fsid.__val[0];
  output->klinux_f_fsid.__val[1] = input->f_fsid.__val[1];
  output->klinux_f_namelen = input->f_namelen;
  output->klinux_f_frsize = input->f_frsize;
  output->klinux_f_flags = input->f_flags;
  memset(output->klinux_f_spare, 0, sizeof(output->klinux_f_spare));
  return true;
}