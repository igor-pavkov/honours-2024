static int selinux_ib_pkey_access(void *ib_sec, u64 subnet_prefix, u16 pkey_val)
{
	struct common_audit_data ad;
	int err;
	u32 sid = 0;
	struct ib_security_struct *sec = ib_sec;
	struct lsm_ibpkey_audit ibpkey;

	err = sel_ib_pkey_sid(subnet_prefix, pkey_val, &sid);
	if (err)
		return err;

	ad.type = LSM_AUDIT_DATA_IBPKEY;
	ibpkey.subnet_prefix = subnet_prefix;
	ibpkey.pkey = pkey_val;
	ad.u.ibpkey = &ibpkey;
	return avc_has_perm(&selinux_state,
			    sec->sid, sid,
			    SECCLASS_INFINIBAND_PKEY,
			    INFINIBAND_PKEY__ACCESS, &ad);
}