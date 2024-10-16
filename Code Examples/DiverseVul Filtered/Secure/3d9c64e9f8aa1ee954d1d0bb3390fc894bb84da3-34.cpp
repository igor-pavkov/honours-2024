DLLEXPORT int tjDecompressHeader3(tjhandle handle,
                                  const unsigned char *jpegBuf,
                                  unsigned long jpegSize, int *width,
                                  int *height, int *jpegSubsamp,
                                  int *jpegColorspace)
{
  int retval = 0;

  getdinstance(handle);
  if ((this->init & DECOMPRESS) == 0)
    _throw("tjDecompressHeader3(): Instance has not been initialized for decompression");

  if (jpegBuf == NULL || jpegSize <= 0 || width == NULL || height == NULL ||
      jpegSubsamp == NULL || jpegColorspace == NULL)
    _throw("tjDecompressHeader3(): Invalid argument");

  if (setjmp(this->jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error. */
    return -1;
  }

  jpeg_mem_src_tj(dinfo, jpegBuf, jpegSize);
  jpeg_read_header(dinfo, TRUE);

  *width = dinfo->image_width;
  *height = dinfo->image_height;
  *jpegSubsamp = getSubsamp(dinfo);
  switch (dinfo->jpeg_color_space) {
  case JCS_GRAYSCALE:  *jpegColorspace = TJCS_GRAY;  break;
  case JCS_RGB:        *jpegColorspace = TJCS_RGB;  break;
  case JCS_YCbCr:      *jpegColorspace = TJCS_YCbCr;  break;
  case JCS_CMYK:       *jpegColorspace = TJCS_CMYK;  break;
  case JCS_YCCK:       *jpegColorspace = TJCS_YCCK;  break;
  default:             *jpegColorspace = -1;  break;
  }

  jpeg_abort_decompress(dinfo);

  if (*jpegSubsamp < 0)
    _throw("tjDecompressHeader3(): Could not determine subsampling type for JPEG image");
  if (*jpegColorspace < 0)
    _throw("tjDecompressHeader3(): Could not determine colorspace of JPEG image");
  if (*width < 1 || *height < 1)
    _throw("tjDecompressHeader3(): Invalid data returned in header");

bailout:
  if (this->jerr.warning) retval = -1;
  return retval;
}