static int writeInt64(u8 *p, i64 i){
#if SQLITE_BYTEORDER==1234 && GCC_VERSION>=4003000
  i = (i64)__builtin_bswap64((u64)i);
  memcpy(p, &i, 8);
#elif SQLITE_BYTEORDER==1234 && MSVC_VERSION>=1300
  i = (i64)_byteswap_uint64((u64)i);
  memcpy(p, &i, 8);
#elif SQLITE_BYTEORDER==4321
  memcpy(p, &i, 8);
#else
  p[0] = (i>>56)&0xFF;
  p[1] = (i>>48)&0xFF;
  p[2] = (i>>40)&0xFF;
  p[3] = (i>>32)&0xFF;
  p[4] = (i>>24)&0xFF;
  p[5] = (i>>16)&0xFF;
  p[6] = (i>> 8)&0xFF;
  p[7] = (i>> 0)&0xFF;
#endif
  return 8;
}