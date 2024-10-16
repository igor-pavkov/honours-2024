static tjhandle _tjInitDecompress(tjinstance *this)
{
  static unsigned char buffer[1];

  /* This is also straight out of example.txt */
  this->dinfo.err = jpeg_std_error(&this->jerr.pub);
  this->jerr.pub.error_exit = my_error_exit;
  this->jerr.pub.output_message = my_output_message;
  this->jerr.emit_message = this->jerr.pub.emit_message;
  this->jerr.pub.emit_message = my_emit_message;
  this->jerr.pub.addon_message_table = turbojpeg_message_table;
  this->jerr.pub.first_addon_message = JMSG_FIRSTADDONCODE;
  this->jerr.pub.last_addon_message = JMSG_LASTADDONCODE;

  if (setjmp(this->jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error. */
    if (this) free(this);
    return NULL;
  }

  jpeg_create_decompress(&this->dinfo);
  /* Make an initial call so it will create the source manager */
  jpeg_mem_src_tj(&this->dinfo, buffer, 1);

  this->init |= DECOMPRESS;
  return (tjhandle)this;
}