static void __generic_bitop_flags(RAnalOp *op) {
	ESIL_A ("0,vf,=,");					// V
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("vf,nf,^,sf,=,");				// S
}