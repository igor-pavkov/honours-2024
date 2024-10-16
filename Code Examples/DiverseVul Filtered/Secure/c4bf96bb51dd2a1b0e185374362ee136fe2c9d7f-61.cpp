ENTROPY_DEBUG(const char * label, unsigned long entropy) {
  const char * const EXPAT_ENTROPY_DEBUG = getenv("EXPAT_ENTROPY_DEBUG");
  if (EXPAT_ENTROPY_DEBUG && ! strcmp(EXPAT_ENTROPY_DEBUG, "1")) {
    fprintf(stderr, "Entropy: %s --> 0x%0*lx (%lu bytes)\n",
        label,
        (int)sizeof(entropy) * 2, entropy,
        (unsigned long)sizeof(entropy));
  }
  return entropy;
}