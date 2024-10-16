PJ_DEF(pj_status_t) pj_stun_msg_create( pj_pool_t *pool,
					unsigned msg_type,
					pj_uint32_t magic,
					const pj_uint8_t tsx_id[12],
					pj_stun_msg **p_msg)
{
    pj_stun_msg *msg;

    PJ_ASSERT_RETURN(pool && msg_type && p_msg, PJ_EINVAL);

    msg = PJ_POOL_ZALLOC_T(pool, pj_stun_msg);
    *p_msg = msg;
    return pj_stun_msg_init(msg, msg_type, magic, tsx_id);
}