static ALWAYS_INLINE double SQRT(double X)
{
	/* If this arch doesn't use IEEE 754 floats, fall back to using libm */
	if (sizeof(float) != 4)
		return sqrt(X);

	/* This avoids needing libm, saves about 0.5k on x86-32 */
	return my_SQRT(X);
}