static int equal_case(const unsigned char *pattern, size_t pattern_len,
                      const unsigned char *subject, size_t subject_len,
                      unsigned int flags)
{
    skip_prefix(&pattern, &pattern_len, subject_len, flags);
    if (pattern_len != subject_len)
        return 0;
    return !memcmp(pattern, subject, pattern_len);
}