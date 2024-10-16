void HeifContext::Image::set_preencoded_hevc_image(const std::vector<uint8_t>& data)
{
  m_heif_context->m_heif_file->add_hvcC_property(m_id);


  // --- parse the h265 stream and set hvcC headers and compressed image data

  int state=0;

  bool first=true;
  bool eof=false;

  int prev_start_code_start = -1; // init to an invalid value, will always be overwritten before use
  int start_code_start;
  int ptr = 0;

  for (;;) {
    bool dump_nal = false;

    uint8_t c = data[ptr++];

    if (state==3) {
      state=0;
    }

    //printf("read c=%02x\n",c);

    if (c==0 && state<=1) {
      state++;
    }
    else if (c==0) {
      // NOP
    }
    else if (c==1 && state==2) {
      start_code_start = ptr - 3;
      dump_nal = true;
      state=3;
    }
    else {
      state=0;
    }

    //printf("-> state= %d\n",state);

    if (ptr == (int)data.size()) {
      start_code_start = (int)data.size();
      dump_nal = true;
      eof = true;
    }

    if (dump_nal) {
      if (first) {
        first = false;
      }
      else {
        std::vector<uint8_t> nal_data;
        size_t length = start_code_start - (prev_start_code_start+3);

        nal_data.resize(length);

        assert(prev_start_code_start>=0);
        memcpy(nal_data.data(), data.data() + prev_start_code_start+3, length);

        int nal_type = (nal_data[0]>>1);

        switch (nal_type) {
        case 0x20:
        case 0x21:
        case 0x22:
          m_heif_context->m_heif_file->append_hvcC_nal_data(m_id, nal_data);
          /*hvcC->append_nal_data(nal_data);*/
          break;

        default: {
          std::vector<uint8_t> nal_data_with_size;
          nal_data_with_size.resize(nal_data.size() + 4);

          memcpy(nal_data_with_size.data()+4, nal_data.data(), nal_data.size());
          nal_data_with_size[0] = ((nal_data.size()>>24) & 0xFF);
          nal_data_with_size[1] = ((nal_data.size()>>16) & 0xFF);
          nal_data_with_size[2] = ((nal_data.size()>> 8) & 0xFF);
          nal_data_with_size[3] = ((nal_data.size()>> 0) & 0xFF);

          m_heif_context->m_heif_file->append_iloc_data(m_id, nal_data_with_size);
        }
          break;
        }
      }

      prev_start_code_start = start_code_start;
    }

    if (eof) {
      break;
    }
  }
}