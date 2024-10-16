static int stbi__bmp_set_mask_defaults(stbi__bmp_data *info, int compress)
{
   // BI_BITFIELDS specifies masks explicitly, don't override
   if (compress == 3)
      return 1;

   if (compress == 0) {
      if (info->bpp == 16) {
         info->mr = 31u << 10;
         info->mg = 31u <<  5;
         info->mb = 31u <<  0;
      } else if (info->bpp == 32) {
         info->mr = 0xffu << 16;
         info->mg = 0xffu <<  8;
         info->mb = 0xffu <<  0;
         info->ma = 0xffu << 24;
         info->all_a = 0; // if all_a is 0 at end, then we loaded alpha channel but it was all 0
      } else {
         // otherwise, use defaults, which is all-0
         info->mr = info->mg = info->mb = info->ma = 0;
      }
      return 1;
   }
   return 0; // error
}