PJ_DEF(pj_status_t) pj_stun_errcode_attr_create(pj_pool_t *pool,
						int err_code,
						const pj_str_t *err_reason,
						pj_stun_errcode_attr **p_attr)
{
    pj_stun_errcode_attr *attr;
    char err_buf[80];
    pj_str_t str;

    PJ_ASSERT_RETURN(pool && err_code && p_attr, PJ_EINVAL);

    if (err_reason == NULL) {
	str = pj_stun_get_err_reason(err_code);
	if (str.slen == 0) {
	    str.slen = pj_ansi_snprintf(err_buf, sizeof(err_buf),
				        "Unknown error %d", err_code);
	    str.ptr = err_buf;
	}
	err_reason = &str;
    }

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_errcode_attr);
    INIT_ATTR(attr, PJ_STUN_ATTR_ERROR_CODE, 4+err_reason->slen);
    attr->err_code = err_code;
    pj_strdup(pool, &attr->reason, err_reason);

    *p_attr = attr;

    return PJ_SUCCESS;
}