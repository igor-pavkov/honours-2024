void decoder_context::process_picture_order_count(slice_segment_header* hdr)
{
  loginfo(LogHeaders,"POC computation. lsb:%d prev.pic.lsb:%d msb:%d\n",
          hdr->slice_pic_order_cnt_lsb,
          prevPicOrderCntLsb,
          PicOrderCntMsb);

  if (isIRAP(nal_unit_type) &&
      NoRaslOutputFlag)
    {
      PicOrderCntMsb=0;


      // flush all images from reorder buffer

      flush_reorder_buffer_at_this_frame = true;
      //ctx->dpb.flush_reorder_buffer();
    }
  else
    {
      int MaxPicOrderCntLsb = current_sps->MaxPicOrderCntLsb;

      if ((hdr->slice_pic_order_cnt_lsb < prevPicOrderCntLsb) &&
          (prevPicOrderCntLsb - hdr->slice_pic_order_cnt_lsb) >= MaxPicOrderCntLsb/2) {
        PicOrderCntMsb = prevPicOrderCntMsb + MaxPicOrderCntLsb;
      }
      else if ((hdr->slice_pic_order_cnt_lsb > prevPicOrderCntLsb) &&
               (hdr->slice_pic_order_cnt_lsb - prevPicOrderCntLsb) > MaxPicOrderCntLsb/2) {
        PicOrderCntMsb = prevPicOrderCntMsb - MaxPicOrderCntLsb;
      }
      else {
        PicOrderCntMsb = prevPicOrderCntMsb;
      }
    }

  img->PicOrderCntVal = PicOrderCntMsb + hdr->slice_pic_order_cnt_lsb;
  img->picture_order_cnt_lsb = hdr->slice_pic_order_cnt_lsb;

  loginfo(LogHeaders,"POC computation. new msb:%d POC=%d\n",
          PicOrderCntMsb,
          img->PicOrderCntVal);

  if (img->nal_hdr.nuh_temporal_id==0 &&
      !isSublayerNonReference(nal_unit_type) &&
      !isRASL(nal_unit_type) &&
      !isRADL(nal_unit_type))
    {
      loginfo(LogHeaders,"set prevPicOrderCntLsb/Msb\n");

      prevPicOrderCntLsb = hdr->slice_pic_order_cnt_lsb;
      prevPicOrderCntMsb = PicOrderCntMsb;
    }
}