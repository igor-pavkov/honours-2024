static void* clone_unknown_attr(pj_pool_t *pool, const void *src)
{
    pj_stun_unknown_attr *dst = PJ_POOL_ALLOC_T(pool, pj_stun_unknown_attr);

    pj_memcpy(dst, src, sizeof(pj_stun_unknown_attr));
    
    return (void*)dst;
}