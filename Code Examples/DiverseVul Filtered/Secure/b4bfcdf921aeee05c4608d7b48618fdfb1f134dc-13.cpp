static void canonicalize_inheritance_bits(struct security_descriptor *psd)
{
	bool set_auto_inherited = false;

	/*
	 * We need to filter out the
	 * SEC_DESC_DACL_AUTO_INHERITED|SEC_DESC_DACL_AUTO_INHERIT_REQ
	 * bits. If both are set we store SEC_DESC_DACL_AUTO_INHERITED
	 * as this alters whether SEC_ACE_FLAG_INHERITED_ACE is set
	 * when an ACE is inherited. Otherwise we zero these bits out.
	 * See:
	 *
	 * http://social.msdn.microsoft.com/Forums/eu/os_fileservices/thread/11f77b68-731e-407d-b1b3-064750716531
	 *
	 * for details.
	 */

	if ((psd->type & (SEC_DESC_DACL_AUTO_INHERITED|SEC_DESC_DACL_AUTO_INHERIT_REQ))
			== (SEC_DESC_DACL_AUTO_INHERITED|SEC_DESC_DACL_AUTO_INHERIT_REQ)) {
		set_auto_inherited = true;
	}

	psd->type &= ~(SEC_DESC_DACL_AUTO_INHERITED|SEC_DESC_DACL_AUTO_INHERIT_REQ);
	if (set_auto_inherited) {
		psd->type |= SEC_DESC_DACL_AUTO_INHERITED;
	}
}