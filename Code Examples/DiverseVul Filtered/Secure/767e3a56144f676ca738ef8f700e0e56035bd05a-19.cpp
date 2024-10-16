    static void* memcpy_jump(void *dest, const void *src, size_t n)
    {
        memcpy(dest, src, n);
        int8_t * byte_pointer = static_cast<int8_t*>(dest);
        return static_cast<void*>(byte_pointer + n);
    }