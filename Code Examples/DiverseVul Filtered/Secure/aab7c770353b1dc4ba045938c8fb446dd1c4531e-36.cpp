 BIGNUM *BN_secure_new(void)
 {
     BIGNUM *ret = BN_new();
     if (ret != NULL)
         ret->flags |= BN_FLG_SECURE;
     return (ret);
 }