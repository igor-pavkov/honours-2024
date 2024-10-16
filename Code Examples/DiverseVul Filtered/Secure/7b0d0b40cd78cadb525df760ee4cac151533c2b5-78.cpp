static int cred_has_capability(const struct cred *cred,
			       int cap, int audit)
{
	struct common_audit_data ad;
	struct av_decision avd;
	u16 sclass;
	u32 sid = cred_sid(cred);
	u32 av = CAP_TO_MASK(cap);
	int rc;

	ad.type = LSM_AUDIT_DATA_CAP;
	ad.u.cap = cap;

	switch (CAP_TO_INDEX(cap)) {
	case 0:
		sclass = SECCLASS_CAPABILITY;
		break;
	case 1:
		sclass = SECCLASS_CAPABILITY2;
		break;
	default:
		printk(KERN_ERR
		       "SELinux:  out of range capability %d\n", cap);
		BUG();
		return -EINVAL;
	}

	rc = avc_has_perm_noaudit(sid, sid, sclass, av, 0, &avd);
	if (audit == SECURITY_CAP_AUDIT) {
		int rc2 = avc_audit(sid, sid, sclass, av, &avd, rc, &ad);
		if (rc2)
			return rc2;
	}
	return rc;
}