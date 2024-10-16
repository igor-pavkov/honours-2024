inline uint64_t kLinuxCpuWordNum(int cpu) {
  return cpu / (8 * sizeof(klinux_cpu_set_word));
}