static Bigint * pow5mult(Bigint *b, int k)
{
	Bigint *b1, *p5, *p51;
	int i;
	static int p05[3] = { 5, 25, 125 };

	_THREAD_PRIVATE_MUTEX_LOCK(pow5mult_mutex);
	if ((i = k & 3)) {
		b = multadd(b, p05[i-1], 0);
	}

	if (!(k >>= 2)) {
		_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);
		return b;
	}
	if (!(p5 = p5s)) {
		/* first time */
		p5 = p5s = i2b(625);
		p5->next = 0;
	}
	for(;;) {
		if (k & 1) {
			b1 = mult(b, p5);
			Bfree(b);
			b = b1;
		}
		if (!(k >>= 1)) {
			break;
		}
		if (!(p51 = p5->next)) {
			if (!(p51 = p5->next)) {
				p51 = p5->next = mult(p5,p5);
				p51->next = 0;
			}
		}
		p5 = p51;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);
	return b;
}