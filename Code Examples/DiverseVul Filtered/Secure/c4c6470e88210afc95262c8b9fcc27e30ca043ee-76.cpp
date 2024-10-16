static void t_blosc_do_job(void *ctxt)
{
  struct thread_context* thcontext = (struct thread_context*)ctxt;
  blosc2_context* context = thcontext->parent_context;
  int32_t cbytes;
  int32_t ntdest;
  int32_t tblocks;               /* number of blocks per thread */
  int32_t tblock;                /* limit block on a thread */
  int32_t nblock_;              /* private copy of nblock */
  int32_t bsize;
  int32_t leftoverblock;
  /* Parameters for threads */
  int32_t blocksize;
  int32_t ebsize;
  int32_t srcsize;
  bool compress = context->do_compress != 0;
  int32_t maxbytes;
  int32_t nblocks;
  int32_t leftover;
  int32_t leftover2;
  int32_t* bstarts;
  const uint8_t* src;
  uint8_t* dest;
  uint8_t* tmp;
  uint8_t* tmp2;
  uint8_t* tmp3;

  /* Get parameters for this thread before entering the main loop */
  blocksize = context->blocksize;
  ebsize = blocksize + context->typesize * sizeof(int32_t);
  maxbytes = context->destsize;
  nblocks = context->nblocks;
  leftover = context->leftover;
  bstarts = context->bstarts;
  src = context->src;
  srcsize = context->srcsize;
  dest = context->dest;

  /* Resize the temporaries if needed */
  if (blocksize != thcontext->tmp_blocksize) {
    my_free(thcontext->tmp);
    thcontext->tmp_nbytes = (size_t)3 * context->blocksize + ebsize;
    thcontext->tmp = my_malloc(thcontext->tmp_nbytes);
    thcontext->tmp2 = thcontext->tmp + blocksize;
    thcontext->tmp3 = thcontext->tmp + blocksize + ebsize;
    thcontext->tmp4 = thcontext->tmp + 2 * blocksize + ebsize;
    thcontext->tmp_blocksize = blocksize;
  }

  tmp = thcontext->tmp;
  tmp2 = thcontext->tmp2;
  tmp3 = thcontext->tmp3;

  // Determine whether we can do a static distribution of workload among different threads
  bool memcpyed = context->header_flags & (uint8_t)BLOSC_MEMCPYED;
  bool static_schedule = (!compress || memcpyed) && context->block_maskout == NULL;
  if (static_schedule) {
      /* Blocks per thread */
      tblocks = nblocks / context->nthreads;
      leftover2 = nblocks % context->nthreads;
      tblocks = (leftover2 > 0) ? tblocks + 1 : tblocks;
      nblock_ = thcontext->tid * tblocks;
      tblock = nblock_ + tblocks;
      if (tblock > nblocks) {
          tblock = nblocks;
      }
  }
  else {
    // Use dynamic schedule via a queue.  Get the next block.
    pthread_mutex_lock(&context->count_mutex);
    context->thread_nblock++;
    nblock_ = context->thread_nblock;
    pthread_mutex_unlock(&context->count_mutex);
    tblock = nblocks;
  }

  /* Loop over blocks */
  leftoverblock = 0;
  while ((nblock_ < tblock) && (context->thread_giveup_code > 0)) {
    bsize = blocksize;
    if (nblock_ == (nblocks - 1) && (leftover > 0)) {
      bsize = leftover;
      leftoverblock = 1;
    }
    if (compress) {
      if (memcpyed) {
        if (!context->prefilter) {
          /* We want to memcpy only */
          memcpy(dest + BLOSC_MAX_OVERHEAD + nblock_ * blocksize,
                 src + nblock_ * blocksize, (unsigned int) bsize);
          cbytes = (int32_t) bsize;
        }
        else {
          /* Only the prefilter has to be executed, and this is done in blosc_c().
           * However, no further actions are needed, so we can put the result
           * directly in dest. */
          cbytes = blosc_c(thcontext, bsize, leftoverblock, 0,
                           ebsize, src, nblock_ * blocksize,
                           dest + BLOSC_MAX_OVERHEAD + nblock_ * blocksize,
                           tmp, tmp3);
        }
      }
      else {
        /* Regular compression */
        cbytes = blosc_c(thcontext, bsize, leftoverblock, 0,
                          ebsize, src, nblock_ * blocksize, tmp2, tmp, tmp3);
      }
    }
    else {
      if (memcpyed) {
        /* We want to memcpy only */
        if (srcsize < BLOSC_MAX_OVERHEAD + (nblock_ * blocksize) + bsize) {
          /* Not enough input to copy data */

          cbytes = -1;
        } else {
          memcpy(dest + nblock_ * blocksize,
                  src + BLOSC_MAX_OVERHEAD + nblock_ * blocksize, (unsigned int)bsize);
          cbytes = (int32_t)bsize;
        }
      }
      else {
        if (srcsize < (int32_t)(BLOSC_MAX_OVERHEAD + (sizeof(int32_t) * nblocks))) {
          /* Not enough input to read all `bstarts` */
          cbytes = -1;
        } else {
          cbytes = blosc_d(thcontext, bsize, leftoverblock,
                            src, srcsize, sw32_(bstarts + nblock_),
                            dest, nblock_ * blocksize, tmp, tmp2);
        }
      }
    }

    /* Check whether current thread has to giveup */
    if (context->thread_giveup_code <= 0) {
      break;
    }

    /* Check results for the compressed/decompressed block */
    if (cbytes < 0) {            /* compr/decompr failure */
      /* Set giveup_code error */
      pthread_mutex_lock(&context->count_mutex);
      context->thread_giveup_code = cbytes;
      pthread_mutex_unlock(&context->count_mutex);
      break;
    }

    if (compress && !memcpyed) {
      /* Start critical section */
      pthread_mutex_lock(&context->count_mutex);
      ntdest = context->output_bytes;
      // Note: do not use a typical local dict_training variable here
      // because it is probably cached from previous calls if the number of
      // threads does not change (the usual thing).
      if (!(context->use_dict && context->dict_cdict == NULL)) {
        _sw32(bstarts + nblock_, (int32_t) ntdest);
      }

      if ((cbytes == 0) || (ntdest + cbytes > maxbytes)) {
        context->thread_giveup_code = 0;  /* uncompressible buf */
        pthread_mutex_unlock(&context->count_mutex);
        break;
      }
      context->thread_nblock++;
      nblock_ = context->thread_nblock;
      context->output_bytes += cbytes;
      pthread_mutex_unlock(&context->count_mutex);
      /* End of critical section */

      /* Copy the compressed buffer to destination */
      memcpy(dest + ntdest, tmp2, (unsigned int) cbytes);
    }
    else if (static_schedule) {
      nblock_++;
    }
    else {
      pthread_mutex_lock(&context->count_mutex);
      context->thread_nblock++;
      nblock_ = context->thread_nblock;
      context->output_bytes += cbytes;
      pthread_mutex_unlock(&context->count_mutex);
    }

  } /* closes while (nblock_) */

  if (static_schedule) {
    context->output_bytes = context->sourcesize;
    if (compress) {
      context->output_bytes += BLOSC_MAX_OVERHEAD;
    }
  }

}