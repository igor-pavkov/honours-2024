ZEND_API void zend_hash_bucket_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;
	zend_string *key;

	ZVAL_COPY_VALUE(&val, &p->val);
	h = p->h;
	key = p->key;

	ZVAL_COPY_VALUE(&p->val, &q->val);
	p->h = q->h;
	p->key = q->key;

	ZVAL_COPY_VALUE(&q->val, &val);
	q->h = h;
	q->key = key;
}