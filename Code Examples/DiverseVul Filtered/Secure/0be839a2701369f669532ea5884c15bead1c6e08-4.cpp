static inline bool is_zero_range(uint8_t *p, uint64_t size)
{
    return buffer_find_nonzero_offset(p, size) == size;
}