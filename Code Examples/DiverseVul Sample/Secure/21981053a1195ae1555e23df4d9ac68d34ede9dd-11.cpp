void slap_free_ctrls(
	Operation *op,
	LDAPControl **ctrls )
{
	int i;

	if( ctrls == op->o_ctrls ) {
		if( op->o_assertion != NULL ) {
			filter_free_x( op, op->o_assertion, 1 );
			op->o_assertion = NULL;
		}
		if( op->o_vrFilter != NULL) {
			vrFilter_free( op, op->o_vrFilter );
			op->o_vrFilter = NULL;
		}
		if( op->o_preread_attrs != NULL ) {
			op->o_tmpfree( op->o_preread_attrs, op->o_tmpmemctx );
			op->o_preread_attrs = NULL;
		}
		if( op->o_postread_attrs != NULL ) {
			op->o_tmpfree( op->o_postread_attrs, op->o_tmpmemctx );
			op->o_postread_attrs = NULL;
		}
		if( op->o_pagedresults_state != NULL ) {
			op->o_tmpfree( op->o_pagedresults_state, op->o_tmpmemctx );
			op->o_pagedresults_state = NULL;
		}
	}

	for (i=0; ctrls[i]; i++) {
		op->o_tmpfree(ctrls[i], op->o_tmpmemctx );
	}
	op->o_tmpfree( ctrls, op->o_tmpmemctx );
}