static void pharobj_set_compression(HashTable *manifest, uint32_t compress) /* {{{ */
{
	zend_hash_apply_with_argument(manifest, phar_set_compression, &compress);
}