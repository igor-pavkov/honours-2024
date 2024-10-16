bool decoder_context::construct_reference_picture_lists(slice_segment_header* hdr)
{
  int NumPocTotalCurr = hdr->NumPocTotalCurr;
  int NumRpsCurrTempList0 = libde265_max(hdr->num_ref_idx_l0_active, NumPocTotalCurr);

  // TODO: fold code for both lists together

  int RefPicListTemp0[3*MAX_NUM_REF_PICS]; // TODO: what would be the correct maximum ?
  int RefPicListTemp1[3*MAX_NUM_REF_PICS]; // TODO: what would be the correct maximum ?
  char isLongTerm[2][3*MAX_NUM_REF_PICS];

  memset(isLongTerm,0,2*3*MAX_NUM_REF_PICS);

  /* --- Fill RefPicListTmp0 with reference pictures in this order:
     1) short term, past POC
     2) short term, future POC
     3) long term
  */

  int rIdx=0;
  while (rIdx < NumRpsCurrTempList0) {
    for (int i=0;i<NumPocStCurrBefore && rIdx<NumRpsCurrTempList0; rIdx++,i++)
      RefPicListTemp0[rIdx] = RefPicSetStCurrBefore[i];

    for (int i=0;i<NumPocStCurrAfter && rIdx<NumRpsCurrTempList0; rIdx++,i++)
      RefPicListTemp0[rIdx] = RefPicSetStCurrAfter[i];

    for (int i=0;i<NumPocLtCurr && rIdx<NumRpsCurrTempList0; rIdx++,i++) {
      RefPicListTemp0[rIdx] = RefPicSetLtCurr[i];
      isLongTerm[0][rIdx] = true;
    }

    // This check is to prevent an endless loop when no images are added above.
    if (rIdx==0) {
      add_warning(DE265_WARNING_FAULTY_REFERENCE_PICTURE_LIST, false);
      return false;
    }
  }

  /*
  if (hdr->num_ref_idx_l0_active > 16) {
    add_warning(DE265_WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED, false);
    return false;
  }
  */

  assert(hdr->num_ref_idx_l0_active <= 16);
  for (rIdx=0; rIdx<hdr->num_ref_idx_l0_active; rIdx++) {
    int idx = hdr->ref_pic_list_modification_flag_l0 ? hdr->list_entry_l0[rIdx] : rIdx;

    hdr->RefPicList[0][rIdx] = RefPicListTemp0[idx];
    hdr->LongTermRefPic[0][rIdx] = isLongTerm[0][idx];

    // remember POC of referenced image (needed in motion.c, derive_collocated_motion_vector)
    de265_image* img_0_rIdx = dpb.get_image(hdr->RefPicList[0][rIdx]);
    if (img_0_rIdx==NULL) {
      return false;
    }
    hdr->RefPicList_POC[0][rIdx] = img_0_rIdx->PicOrderCntVal;
    hdr->RefPicList_PicState[0][rIdx] = img_0_rIdx->PicState;
  }


  /* --- Fill RefPicListTmp1 with reference pictures in this order:
     1) short term, future POC
     2) short term, past POC
     3) long term
  */

  if (hdr->slice_type == SLICE_TYPE_B) {
    int NumRpsCurrTempList1 = libde265_max(hdr->num_ref_idx_l1_active, NumPocTotalCurr);

    int rIdx=0;
    while (rIdx < NumRpsCurrTempList1) {
      for (int i=0;i<NumPocStCurrAfter && rIdx<NumRpsCurrTempList1; rIdx++,i++) {
        RefPicListTemp1[rIdx] = RefPicSetStCurrAfter[i];
      }

      for (int i=0;i<NumPocStCurrBefore && rIdx<NumRpsCurrTempList1; rIdx++,i++) {
        RefPicListTemp1[rIdx] = RefPicSetStCurrBefore[i];
      }

      for (int i=0;i<NumPocLtCurr && rIdx<NumRpsCurrTempList1; rIdx++,i++) {
        RefPicListTemp1[rIdx] = RefPicSetLtCurr[i];
        isLongTerm[1][rIdx] = true;
      }

      // This check is to prevent an endless loop when no images are added above.
      if (rIdx==0) {
        add_warning(DE265_WARNING_FAULTY_REFERENCE_PICTURE_LIST, false);
        return false;
      }
    }

    if (hdr->num_ref_idx_l0_active > 16) {
    add_warning(DE265_WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED, false);
    return false;
  }

    assert(hdr->num_ref_idx_l1_active <= 16);
    for (rIdx=0; rIdx<hdr->num_ref_idx_l1_active; rIdx++) {
      int idx = hdr->ref_pic_list_modification_flag_l1 ? hdr->list_entry_l1[rIdx] : rIdx;

      hdr->RefPicList[1][rIdx] = RefPicListTemp1[idx];
      hdr->LongTermRefPic[1][rIdx] = isLongTerm[1][idx];

      // remember POC of referenced imaged (needed in motion.c, derive_collocated_motion_vector)
      de265_image* img_1_rIdx = dpb.get_image(hdr->RefPicList[1][rIdx]);
      if (img_1_rIdx == NULL) { return false; }
      hdr->RefPicList_POC[1][rIdx] = img_1_rIdx->PicOrderCntVal;
      hdr->RefPicList_PicState[1][rIdx] = img_1_rIdx->PicState;
    }
  }


  // show reference picture lists

  loginfo(LogHeaders,"RefPicList[0] =");
  for (rIdx=0; rIdx<hdr->num_ref_idx_l0_active; rIdx++) {
    loginfo(LogHeaders,"* [%d]=%d (LT=%d)",
            hdr->RefPicList[0][rIdx],
            hdr->RefPicList_POC[0][rIdx],
            hdr->LongTermRefPic[0][rIdx]
            );
  }
  loginfo(LogHeaders,"*\n");

  if (hdr->slice_type == SLICE_TYPE_B) {
    loginfo(LogHeaders,"RefPicList[1] =");
    for (rIdx=0; rIdx<hdr->num_ref_idx_l1_active; rIdx++) {
      loginfo(LogHeaders,"* [%d]=%d (LT=%d)",
              hdr->RefPicList[1][rIdx],
              hdr->RefPicList_POC[1][rIdx],
              hdr->LongTermRefPic[1][rIdx]
              );
    }
    loginfo(LogHeaders,"*\n");
  }

  return true;
}