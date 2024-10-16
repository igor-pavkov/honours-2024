inline bool ReadLimiter::canRead(WordCount64 amount, Arena* arena) {
  // Be careful not to store an underflowed value into `limit`, even if multiple threads are
  // decrementing it.
  uint64_t current = limit;
  if (KJ_UNLIKELY(unbound(amount / WORDS) > current)) {
    arena->reportReadLimitReached();
    return false;
  } else {
    limit = current - unbound(amount / WORDS);
    return true;
  }
}