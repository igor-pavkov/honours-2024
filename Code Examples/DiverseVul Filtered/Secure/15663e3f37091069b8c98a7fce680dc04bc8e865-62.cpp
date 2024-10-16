PJ_DEF(pj_status_t) pj_stun_msg_add_msgint_attr(pj_pool_t *pool,
						pj_stun_msg *msg)
{
    pj_stun_msgint_attr *attr = NULL;
    pj_status_t status;

    status = pj_stun_msgint_attr_create(pool, &attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &attr->hdr);
}