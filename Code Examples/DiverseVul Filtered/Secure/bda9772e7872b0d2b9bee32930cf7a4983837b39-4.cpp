inline klinux_cpu_set_word kLinuxCpuBitNum(int cpu) {
  return cpu % (8 * sizeof(klinux_cpu_set_word));
}