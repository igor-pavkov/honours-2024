int evm_calc_hmac(struct dentry *dentry, const char *req_xattr_name,
		  const char *req_xattr_value, size_t req_xattr_value_len,
		  char *digest)
{
	return evm_calc_hmac_or_hash(dentry, req_xattr_name, req_xattr_value,
				req_xattr_value_len, EVM_XATTR_HMAC, digest);
}