static void* clone_binary_attr(pj_pool_t *pool, const void *src)
{
    const pj_stun_binary_attr *asrc = (const pj_stun_binary_attr*)src;
    pj_stun_binary_attr *dst = PJ_POOL_ALLOC_T(pool, pj_stun_binary_attr);

    pj_memcpy(dst, src, sizeof(pj_stun_binary_attr));

    if (asrc->length) {
	dst->data = (pj_uint8_t*) pj_pool_alloc(pool, asrc->length);
	pj_memcpy(dst->data, asrc->data, asrc->length);
    }

    return (void*)dst;
}