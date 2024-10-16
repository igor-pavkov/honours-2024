int kLinuxCpuSetCheckBit(int cpu, klinux_cpu_set_t *set) {
  return (set->words[kLinuxCpuWordNum(cpu)] &
          (static_cast<klinux_cpu_set_word>(1) << kLinuxCpuBitNum(cpu))) != 0;
}