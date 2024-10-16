bool MemoryLoggingEnabled() {
  static bool memory_logging_enabled = LogMemory::IsEnabled();
  return memory_logging_enabled;
}