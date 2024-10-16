PJ_DEF(pj_status_t) pj_stun_uint64_attr_create(pj_pool_t *pool,
					       int attr_type,
					       const pj_timestamp *value,
					       pj_stun_uint64_attr **p_attr)
{
    pj_stun_uint64_attr *attr;

    PJ_ASSERT_RETURN(pool && p_attr, PJ_EINVAL);

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_uint64_attr);
    INIT_ATTR(attr, attr_type, 8);

    if (value) {
	attr->value.u32.hi = value->u32.hi;
	attr->value.u32.lo = value->u32.lo;
    }

    *p_attr = attr;

    return PJ_SUCCESS;
}