static void my_free(void* block) {
#if defined(_WIN32)
  _aligned_free(block);
#else
  free(block);
#endif  /* _WIN32 */
}