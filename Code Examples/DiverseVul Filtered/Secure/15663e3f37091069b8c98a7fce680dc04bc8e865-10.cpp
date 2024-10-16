PJ_DEF(pj_status_t) pj_stun_msg_add_errcode_attr(pj_pool_t *pool,
						 pj_stun_msg *msg,
						 int err_code,
						 const pj_str_t *err_reason)
{
    pj_stun_errcode_attr *err_attr = NULL;
    pj_status_t status;

    status = pj_stun_errcode_attr_create(pool, err_code, err_reason,
					 &err_attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &err_attr->hdr);
}