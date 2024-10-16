static inline u32 dp_to_sp(u64 fprd)
{
	u32 fprs;

	preempt_disable();
	enable_kernel_fp();
	asm ("lfd%U1%X1 0,%1; stfs%U0%X0 0,%0" : "=m" (fprs) : "m" (fprd)
	     : "fr0");
	preempt_enable();
	return fprs;
}