static int pharobj_cancompress(HashTable *manifest TSRMLS_DC) /* {{{ */
{
	int test;

	test = 1;
	zend_hash_apply_with_argument(manifest, phar_test_compression, &test TSRMLS_CC);
	return test;
}