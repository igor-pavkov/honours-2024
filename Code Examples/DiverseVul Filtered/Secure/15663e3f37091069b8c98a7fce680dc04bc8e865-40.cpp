PJ_DEF(pj_status_t) pj_stun_sockaddr_attr_create(pj_pool_t *pool,
						 int attr_type,
						 pj_bool_t xor_ed,
						 const pj_sockaddr_t *addr,
						 unsigned addr_len,
						 pj_stun_sockaddr_attr **p_attr)
{
    pj_stun_sockaddr_attr *attr;

    PJ_ASSERT_RETURN(pool && p_attr, PJ_EINVAL);
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_sockaddr_attr);
    *p_attr = attr;
    return pj_stun_sockaddr_attr_init(attr, attr_type, xor_ed, 
				      addr, addr_len);
}