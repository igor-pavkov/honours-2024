static cdk_error_t parse_sig_subpackets(cdk_pkt_signature_t sig)
{
	cdk_subpkt_t node;

	/* Setup the standard packet entries, so we can use V4
	   signatures similar to V3. */
	for (node = sig->unhashed; node; node = node->next) {
		if (node->type == CDK_SIGSUBPKT_ISSUER && node->size >= 8) {
			sig->keyid[0] = _cdk_buftou32(node->d);
			sig->keyid[1] = _cdk_buftou32(node->d + 4);
		} else if (node->type == CDK_SIGSUBPKT_EXPORTABLE
			   && node->d[0] == 0) {
			/* Sometimes this packet might be placed in the unhashed area */
			sig->flags.exportable = 0;
		}
	}
	for (node = sig->hashed; node; node = node->next) {
		if (node->type == CDK_SIGSUBPKT_SIG_CREATED
		    && node->size >= 4)
			sig->timestamp = _cdk_buftou32(node->d);
		else if (node->type == CDK_SIGSUBPKT_SIG_EXPIRE
			 && node->size >= 4) {
			sig->expiredate = _cdk_buftou32(node->d);
			if (sig->expiredate > 0
			    && sig->expiredate < (u32) gnutls_time(NULL))
				sig->flags.expired = 1;
		} else if (node->type == CDK_SIGSUBPKT_POLICY)
			sig->flags.policy_url = 1;
		else if (node->type == CDK_SIGSUBPKT_NOTATION)
			sig->flags.notation = 1;
		else if (node->type == CDK_SIGSUBPKT_REVOCABLE
			 && node->d[0] == 0)
			sig->flags.revocable = 0;
		else if (node->type == CDK_SIGSUBPKT_EXPORTABLE
			 && node->d[0] == 0)
			sig->flags.exportable = 0;
	}
	if (sig->sig_class == 0x1F) {
		cdk_desig_revoker_t r, rnode;

		for (node = sig->hashed; node; node = node->next) {
			if (node->type == CDK_SIGSUBPKT_REV_KEY) {
				if (node->size < 22)
					continue;
				rnode = cdk_calloc(1, sizeof *rnode);
				if (!rnode)
					return CDK_Out_Of_Core;
				rnode->r_class = node->d[0];
				rnode->algid = node->d[1];
				memcpy(rnode->fpr, node->d + 2,
				       KEY_FPR_LEN);
				if (!sig->revkeys)
					sig->revkeys = rnode;
				else {
					for (r = sig->revkeys; r->next;
					     r = r->next);
					r->next = rnode;
				}
			}
		}
	}

	return 0;
}