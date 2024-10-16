void decoder_context::process_reference_picture_set(slice_segment_header* hdr)
{
  std::vector<int> removeReferencesList;

  const int currentID = img->get_ID();


  if (isIRAP(nal_unit_type) && NoRaslOutputFlag) {

    int currentPOC = img->PicOrderCntVal;

    // reset DPB

    /* The standard says: "When the current picture is an IRAP picture with NoRaslOutputFlag
       equal to 1, all reference pictures currently in the DPB (if any) are marked as
       "unused for reference".

       This seems to be wrong as it also throws out the first CRA picture in a stream like
       RAP_A (decoding order: CRA,POC=64, RASL,POC=60). Removing only the pictures with
       lower POCs seems to be compliant to the reference decoder.
    */

    for (int i=0;i<dpb.size();i++) {
      de265_image* img = dpb.get_image(i);

      if (img->PicState != UnusedForReference &&
          img->PicOrderCntVal < currentPOC &&
          img->removed_at_picture_id > img->get_ID()) {

        removeReferencesList.push_back(img->get_ID());
        img->removed_at_picture_id = img->get_ID();

        //printf("will remove ID %d (a)\n",img->get_ID());
      }
    }
  }


  if (isIDR(nal_unit_type)) {

    // clear all reference pictures

    NumPocStCurrBefore = 0;
    NumPocStCurrAfter = 0;
    NumPocStFoll = 0;
    NumPocLtCurr = 0;
    NumPocLtFoll = 0;
  }
  else {
    const ref_pic_set* rps = &hdr->CurrRps;

    // (8-98)

    int i,j,k;

    // scan ref-pic-set for smaller POCs and fill into PocStCurrBefore / PocStFoll

    for (i=0, j=0, k=0;
         i<rps->NumNegativePics;
         i++)
      {
        if (rps->UsedByCurrPicS0[i]) {
          PocStCurrBefore[j++] = img->PicOrderCntVal + rps->DeltaPocS0[i];
          //printf("PocStCurrBefore = %d\n",PocStCurrBefore[j-1]);
        }
        else {
          PocStFoll[k++] = img->PicOrderCntVal + rps->DeltaPocS0[i];
        }
      }

    NumPocStCurrBefore = j;


    // scan ref-pic-set for larger POCs and fill into PocStCurrAfter / PocStFoll

    for (i=0, j=0;
         i<rps->NumPositivePics;
         i++)
      {
        if (rps->UsedByCurrPicS1[i]) {
          PocStCurrAfter[j++] = img->PicOrderCntVal + rps->DeltaPocS1[i];
          //printf("PocStCurrAfter = %d\n",PocStCurrAfter[j-1]);
        }
        else {
          PocStFoll[k++] = img->PicOrderCntVal + rps->DeltaPocS1[i];
        }
      }

    NumPocStCurrAfter = j;
    NumPocStFoll = k;


    // find used / future long-term references

    for (i=0, j=0, k=0;
         //i<current_sps->num_long_term_ref_pics_sps + hdr->num_long_term_pics;
         i<hdr->num_long_term_sps + hdr->num_long_term_pics;
         i++)
      {
        int pocLt = PocLsbLt[i];

        if (hdr->delta_poc_msb_present_flag[i]) {
          int currentPictureMSB = img->PicOrderCntVal - hdr->slice_pic_order_cnt_lsb;
          pocLt += currentPictureMSB
            - DeltaPocMsbCycleLt[i] * current_sps->MaxPicOrderCntLsb;
        }

        if (UsedByCurrPicLt[i]) {
          PocLtCurr[j] = pocLt;
          CurrDeltaPocMsbPresentFlag[j] = hdr->delta_poc_msb_present_flag[i];
          j++;
        }
        else {
          PocLtFoll[k] = pocLt;
          FollDeltaPocMsbPresentFlag[k] = hdr->delta_poc_msb_present_flag[i];
          k++;
        }
      }

    NumPocLtCurr = j;
    NumPocLtFoll = k;
  }


  // (old 8-99) / (new 8-106)
  // 1.

  std::vector<char> picInAnyList(dpb.size(), false);


  dpb.log_dpb_content();

  for (int i=0;i<NumPocLtCurr;i++) {
    int k;
    if (!CurrDeltaPocMsbPresentFlag[i]) {
      k = dpb.DPB_index_of_picture_with_LSB(PocLtCurr[i], currentID, true);
    }
    else {
      k = dpb.DPB_index_of_picture_with_POC(PocLtCurr[i], currentID, true);
    }

    RefPicSetLtCurr[i] = k; // -1 == "no reference picture"
    if (k>=0) picInAnyList[k]=true;
    else {
      // TODO, CHECK: is it ok that we generate a picture with POC = LSB (PocLtCurr)
      // We do not know the correct MSB
      int concealedPicture = generate_unavailable_reference_picture(current_sps.get(),
                                                                    PocLtCurr[i], true);
      picInAnyList.resize(dpb.size(), false); // adjust size of array to hold new picture

      RefPicSetLtCurr[i] = k = concealedPicture;
      picInAnyList[concealedPicture]=true;
    }

    if (dpb.get_image(k)->integrity != INTEGRITY_CORRECT) {
      img->integrity = INTEGRITY_DERIVED_FROM_FAULTY_REFERENCE;
    }
  }


  for (int i=0;i<NumPocLtFoll;i++) {
    int k;
    if (!FollDeltaPocMsbPresentFlag[i]) {
      k = dpb.DPB_index_of_picture_with_LSB(PocLtFoll[i], currentID, true);
    }
    else {
      k = dpb.DPB_index_of_picture_with_POC(PocLtFoll[i], currentID, true);
    }

    RefPicSetLtFoll[i] = k; // -1 == "no reference picture"
    if (k>=0) picInAnyList[k]=true;
    else {
      int concealedPicture = k = generate_unavailable_reference_picture(current_sps.get(),
                                                                        PocLtFoll[i], true);
      picInAnyList.resize(dpb.size(), false); // adjust size of array to hold new picture

      RefPicSetLtFoll[i] = concealedPicture;
      picInAnyList[concealedPicture]=true;
    }
  }


  // 2. Mark all pictures in RefPicSetLtCurr / RefPicSetLtFoll as UsedForLongTermReference

  for (int i=0;i<NumPocLtCurr;i++) {
    dpb.get_image(RefPicSetLtCurr[i])->PicState = UsedForLongTermReference;
  }

  for (int i=0;i<NumPocLtFoll;i++) {
    dpb.get_image(RefPicSetLtFoll[i])->PicState = UsedForLongTermReference;
  }


  // 3.

  for (int i=0;i<NumPocStCurrBefore;i++) {
    int k = dpb.DPB_index_of_picture_with_POC(PocStCurrBefore[i], currentID);

    //printf("st curr before, poc=%d -> idx=%d\n",PocStCurrBefore[i], k);

    RefPicSetStCurrBefore[i] = k; // -1 == "no reference picture"
    if (k>=0) picInAnyList[k]=true;
    else {
      int concealedPicture = generate_unavailable_reference_picture(current_sps.get(),
                                                                    PocStCurrBefore[i], false);
      RefPicSetStCurrBefore[i] = k = concealedPicture;

      picInAnyList.resize(dpb.size(), false); // adjust size of array to hold new picture
      picInAnyList[concealedPicture] = true;

      //printf("  concealed: %d\n", concealedPicture);
    }

    if (dpb.get_image(k)->integrity != INTEGRITY_CORRECT) {
      img->integrity = INTEGRITY_DERIVED_FROM_FAULTY_REFERENCE;
    }
  }

  for (int i=0;i<NumPocStCurrAfter;i++) {
    int k = dpb.DPB_index_of_picture_with_POC(PocStCurrAfter[i], currentID);

    //printf("st curr after, poc=%d -> idx=%d\n",PocStCurrAfter[i], k);

    RefPicSetStCurrAfter[i] = k; // -1 == "no reference picture"
    if (k>=0) picInAnyList[k]=true;
    else {
      int concealedPicture = generate_unavailable_reference_picture(current_sps.get(),
                                                                    PocStCurrAfter[i], false);
      RefPicSetStCurrAfter[i] = k = concealedPicture;


      picInAnyList.resize(dpb.size(), false); // adjust size of array to hold new picture
      picInAnyList[concealedPicture]=true;

      //printf("  concealed: %d\n", concealedPicture);
    }

    if (dpb.get_image(k)->integrity != INTEGRITY_CORRECT) {
      img->integrity = INTEGRITY_DERIVED_FROM_FAULTY_REFERENCE;
    }
  }

  for (int i=0;i<NumPocStFoll;i++) {
    int k = dpb.DPB_index_of_picture_with_POC(PocStFoll[i], currentID);
    // if (k<0) { assert(false); } // IGNORE

    RefPicSetStFoll[i] = k; // -1 == "no reference picture"
    if (k>=0) picInAnyList[k]=true;
  }

  // 4. any picture that is not marked for reference is put into the "UnusedForReference" state

  for (int i=0;i<dpb.size();i++)
    if (i>=picInAnyList.size() || !picInAnyList[i])        // no reference
      {
        de265_image* dpbimg = dpb.get_image(i);
        if (dpbimg != img &&  // not the current picture
            dpbimg->removed_at_picture_id > img->get_ID()) // has not been removed before
          {
            if (dpbimg->PicState != UnusedForReference) {
              removeReferencesList.push_back(dpbimg->get_ID());
              //printf("will remove ID %d (b)\n",dpbimg->get_ID());

              dpbimg->removed_at_picture_id = img->get_ID();
            }
          }
      }

  hdr->RemoveReferencesList = removeReferencesList;

  //remove_images_from_dpb(hdr->RemoveReferencesList);
}