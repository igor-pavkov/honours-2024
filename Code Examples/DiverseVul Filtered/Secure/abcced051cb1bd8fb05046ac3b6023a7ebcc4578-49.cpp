static Allocator* get_default_cpu_allocator() {
  static Allocator* default_cpu_allocator =
      cpu_allocator(port::kNUMANoAffinity);
  return default_cpu_allocator;
}