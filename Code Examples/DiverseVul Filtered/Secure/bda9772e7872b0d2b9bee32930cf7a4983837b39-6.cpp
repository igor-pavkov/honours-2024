bool FromkLinuxCpuSet(klinux_cpu_set_t *input, cpu_set_t *output) {
  if (!input || !output) {
    return false;
  }

  CPU_ZERO(output);

  for (int cpu = 0; cpu < KLINUX_CPU_SET_MAX_CPUS; cpu++) {
    if (kLinuxCpuSetCheckBit(cpu, input)) {
      CPU_SET(cpu, output);
    }
  }
  return true;
}