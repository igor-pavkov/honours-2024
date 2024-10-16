ZEND_API void zend_hash_bucket_packed_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;

	ZVAL_COPY_VALUE(&val, &p->val);
	h = p->h;

	ZVAL_COPY_VALUE(&p->val, &q->val);
	p->h = q->h;

	ZVAL_COPY_VALUE(&q->val, &val);
	q->h = h;
}