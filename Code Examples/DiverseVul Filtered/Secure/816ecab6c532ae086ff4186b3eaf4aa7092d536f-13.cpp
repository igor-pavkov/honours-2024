static void mng_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  MngInfo
    *mng_info;

  Image
    *image;

  png_size_t
    check;

  register ssize_t
    i;

  i=0;
  mng_info=(MngInfo *) png_get_io_ptr(png_ptr);
  image=(Image *) mng_info->image;
  while (mng_info->bytes_in_read_buffer && length)
  {
    data[i]=mng_info->read_buffer[i];
    mng_info->bytes_in_read_buffer--;
    length--;
    i++;
  }
  if (length != 0)
    {
      check=(png_size_t) ReadBlob(image,(size_t) length,(char *) data);

      if (check != length)
        png_error(png_ptr,"Read Exception");

      if (length == 4)
        {
          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 0))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (memcmp(mng_info->read_buffer,mng_PLTE,4) == 0)
                mng_info->found_empty_plte=MagickTrue;
              if (memcmp(mng_info->read_buffer,mng_IEND,4) == 0)
                {
                  mng_info->found_empty_plte=MagickFalse;
                  mng_info->have_saved_bkgd_index=MagickFalse;
                }
            }

          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 1))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (memcmp(mng_info->read_buffer,mng_bKGD,4) == 0)
                if (mng_info->found_empty_plte)
                  {
                    /*
                      Skip the bKGD data byte and CRC.
                    */
                    check=(png_size_t)
                      ReadBlob(image,5,(char *) mng_info->read_buffer);
                    check=(png_size_t) ReadBlob(image,(size_t) length,
                      (char *) mng_info->read_buffer);
                    mng_info->saved_bkgd_index=mng_info->read_buffer[0];
                    mng_info->have_saved_bkgd_index=MagickTrue;
                    mng_info->bytes_in_read_buffer=0;
                  }
            }
        }
    }
}