int slap_add_ctrls(
	Operation *op,
	SlapReply *rs,
	LDAPControl **ctrls )
{
	int i = 0, j;
	LDAPControl **ctrlsp;

	if ( rs->sr_ctrls ) {
		for ( ; rs->sr_ctrls[ i ]; i++ ) ;
	}

	for ( j=0; ctrls[j]; j++ ) ;

	ctrlsp = op->o_tmpalloc(( i+j+1 )*sizeof(LDAPControl *), op->o_tmpmemctx );
	i = 0;
	if ( rs->sr_ctrls ) {
		for ( ; rs->sr_ctrls[i]; i++ )
			ctrlsp[i] = rs->sr_ctrls[i];
	}
	for ( j=0; ctrls[j]; j++)
		ctrlsp[i++] = ctrls[j];
	ctrlsp[i] = NULL;

	if ( rs->sr_flags & REP_CTRLS_MUSTBEFREED )
		op->o_tmpfree( rs->sr_ctrls, op->o_tmpmemctx );
	rs->sr_ctrls = ctrlsp;
	rs->sr_flags |= REP_CTRLS_MUSTBEFREED;
	return i;
}