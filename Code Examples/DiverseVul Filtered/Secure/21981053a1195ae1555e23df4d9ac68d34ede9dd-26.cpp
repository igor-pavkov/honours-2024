static int parsePreRead (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	return parseReadAttrs( op, rs, ctrl, 0 );
}