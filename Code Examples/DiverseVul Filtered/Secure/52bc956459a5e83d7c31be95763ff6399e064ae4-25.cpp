inline ReadLimiter::ReadLimiter(WordCount64 limit): limit(unbound(limit / WORDS)) {}