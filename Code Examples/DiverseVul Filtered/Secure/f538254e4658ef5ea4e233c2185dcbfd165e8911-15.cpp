void decoder_context::mark_whole_slice_as_processed(image_unit* imgunit,
                                                    slice_unit* sliceunit,
                                                    int progress)
{
  //printf("mark whole slice\n");


  // mark all CTBs upto the next slice segment as processed

  slice_unit* nextSegment = imgunit->get_next_slice_segment(sliceunit);
  if (nextSegment) {
    /*
    printf("mark whole slice between %d and %d\n",
           sliceunit->shdr->slice_segment_address,
           nextSegment->shdr->slice_segment_address);
    */

    for (int ctb=sliceunit->shdr->slice_segment_address;
         ctb < nextSegment->shdr->slice_segment_address;
         ctb++)
      {
        if (ctb >= imgunit->img->number_of_ctbs())
          break;

        imgunit->img->ctb_progress[ctb].set_progress(progress);
      }
  }
}