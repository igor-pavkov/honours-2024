static void* clone_errcode_attr(pj_pool_t *pool, const void *src)
{
    const pj_stun_errcode_attr *asrc = (const pj_stun_errcode_attr*)src;
    pj_stun_errcode_attr *dst = PJ_POOL_ALLOC_T(pool, pj_stun_errcode_attr);

    pj_memcpy(dst, src, sizeof(pj_stun_errcode_attr));
    pj_strdup(pool, &dst->reason, &asrc->reason);

    return (void*)dst;
}