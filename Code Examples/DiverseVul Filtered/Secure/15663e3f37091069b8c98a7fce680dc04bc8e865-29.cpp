static void* clone_uint_attr(pj_pool_t *pool, const void *src)
{
    pj_stun_uint_attr *dst = PJ_POOL_ALLOC_T(pool, pj_stun_uint_attr);

    pj_memcpy(dst, src, sizeof(pj_stun_uint_attr));

    return (void*)dst;
}