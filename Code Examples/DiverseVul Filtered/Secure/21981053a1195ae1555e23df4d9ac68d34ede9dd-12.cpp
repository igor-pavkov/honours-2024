static int parsePostRead (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	return parseReadAttrs( op, rs, ctrl, 1 );
}