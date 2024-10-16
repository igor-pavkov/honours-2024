int rateallocate(jpc_enc_t *enc, unsigned numlyrs, uint_fast32_t *cumlens)
{
	jpc_flt_t lo;
	jpc_flt_t hi;
	jas_stream_t *out;
	uint_fast32_t cumlen;
	jpc_flt_t thresh;
	jpc_flt_t goodthresh;
	int success;
	long pos;
	long oldpos;
	int numiters;

	jpc_enc_tcmpt_t *comp;
	jpc_enc_tcmpt_t *endcomps;
	jpc_enc_rlvl_t *lvl;
	jpc_enc_rlvl_t *endlvls;
	jpc_enc_band_t *band;
	jpc_enc_band_t *endbands;
	jpc_enc_cblk_t *cblk;
	jpc_enc_cblk_t *endcblks;
	jpc_enc_pass_t *pass;
	jpc_enc_pass_t *endpasses;
	jpc_enc_pass_t *pass1;
	jpc_flt_t mxrdslope;
	jpc_flt_t mnrdslope;
	jpc_enc_tile_t *tile;
	jpc_enc_prc_t *prc;
	unsigned prcno;

	JAS_DBGLOG(10, ("starting rate allocation\n"));

	tile = enc->curtile;

	for (unsigned lyrno = 1; lyrno < numlyrs - 1; ++lyrno) {
		assert(cumlens[lyrno - 1] <= cumlens[lyrno]);
	}

	if (!(out = jas_stream_memopen(0, 0))) {
		return -1;
	}


	/* Find minimum and maximum R-D slope values. */
	mnrdslope = DBL_MAX;
	mxrdslope = 0;
	endcomps = &tile->tcmpts[tile->numtcmpts];
	for (comp = tile->tcmpts; comp != endcomps; ++comp) {
		endlvls = &comp->rlvls[comp->numrlvls];
		for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
			if (!lvl->bands) {
				continue;
			}
			endbands = &lvl->bands[lvl->numbands];
			for (band = lvl->bands; band != endbands; ++band) {
				if (!band->data) {
					continue;
				}
				for (prcno = 0, prc = band->prcs; prcno < lvl->numprcs;
				  ++prcno, ++prc) {
					if (!prc->cblks) {
						continue;
					}
					endcblks = &prc->cblks[prc->numcblks];
					for (cblk = prc->cblks; cblk != endcblks; ++cblk) {
						calcrdslopes(cblk);
						endpasses = &cblk->passes[cblk->numpasses];
						for (pass = cblk->passes; pass != endpasses; ++pass) {
							if (pass->rdslope > 0) {
								if (pass->rdslope < mnrdslope) {
									mnrdslope = pass->rdslope;
								}
								if (pass->rdslope > mxrdslope) {
									mxrdslope = pass->rdslope;
								}
							}
						}
					}
				}
			}
		}
	}
	JAS_DBGLOG(10, ("min rdslope = %f max rdslope = %f\n", mnrdslope,
	  mxrdslope));

	jpc_init_t2state(enc, true);

	for (unsigned lyrno = 0; lyrno < numlyrs; ++lyrno) {

		lo = mnrdslope;
		hi = mxrdslope;

		success = 0;
		goodthresh = 0;
		numiters = 0;

		do {

			cumlen = cumlens[lyrno];
			if (cumlen == UINT_FAST32_MAX) {
				/* Only the last layer can be free of a rate
				  constraint (e.g., for lossless coding). */
				assert(lyrno == numlyrs - 1);
				goodthresh = -1;
				success = 1;
				break;
			}

			thresh = (lo + hi) / 2;

			/* Save the tier 2 coding state. */
			jpc_save_t2state(enc);
			oldpos = jas_stream_tell(out);
			assert(oldpos >= 0);

			/* Assign all passes with R-D slopes greater than or
			  equal to the current threshold to this layer. */
			endcomps = &tile->tcmpts[tile->numtcmpts];
			for (comp = tile->tcmpts; comp != endcomps; ++comp) {
				endlvls = &comp->rlvls[comp->numrlvls];
				for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
					if (!lvl->bands) {
						continue;
					}
					endbands = &lvl->bands[lvl->numbands];
					for (band = lvl->bands; band != endbands; ++band) {
						if (!band->data) {
							continue;
						}
						for (prcno = 0, prc = band->prcs; prcno < lvl->numprcs;
						  ++prcno, ++prc) {
							if (!prc->cblks) {
								continue;
							}
							endcblks = &prc->cblks[prc->numcblks];
							for (cblk = prc->cblks; cblk != endcblks; ++cblk) {
								if (cblk->curpass) {
									endpasses = &cblk->passes[cblk->numpasses];
									pass1 = cblk->curpass;
									for (pass = cblk->curpass; pass !=
									  endpasses; ++pass) {
										if (pass->rdslope >= thresh) {
											pass1 = &pass[1];
										}
									}
									for (pass = cblk->curpass; pass != pass1;
									  ++pass) {
										pass->lyrno = lyrno;
									}
									for (; pass != endpasses; ++pass) {
										pass->lyrno = -1;
									}
								}
							}
						}
					}
				}
			}

			/* Perform tier 2 coding. */
			endcomps = &tile->tcmpts[tile->numtcmpts];
			for (comp = tile->tcmpts; comp != endcomps; ++comp) {
				endlvls = &comp->rlvls[comp->numrlvls];
				for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
					if (!lvl->bands) {
						continue;
					}
					for (prcno = 0; prcno < lvl->numprcs; ++prcno) {
						if (jpc_enc_encpkt(enc, out, comp - tile->tcmpts,
						  lvl - comp->rlvls, prcno, lyrno)) {
							return -1;
						}
					}
				}
			}

			pos = jas_stream_tell(out);

			/* Check the rate constraint. */
			assert(pos >= 0);
			if ((uint_fast32_t)pos > cumlen) {
				/* The rate is too high. */
				lo = thresh;
			} else if ((uint_fast32_t)pos <= cumlen) {
				/* The rate is low enough, so try higher. */
				hi = thresh;
				if (!success || thresh < goodthresh) {
					goodthresh = thresh;
					success = 1;
				}
			}

			/* Save the tier 2 coding state. */
			jpc_restore_t2state(enc);
			if (jas_stream_seek(out, oldpos, SEEK_SET) < 0) {
				return -1;
			}

			JAS_DBGLOG(10, ("maxlen=%08ld actuallen=%08ld thresh=%f\n",
			  cumlen, pos, thresh));

			++numiters;
		} while (lo < hi - 1e-3 && numiters < 32);

		if (!success) {
			jas_eprintf("warning: empty layer generated\n");
		}

		JAS_DBGLOG(10, ("success %d goodthresh %f\n", success, goodthresh));

		/* Assign all passes with R-D slopes greater than or
		  equal to the selected threshold to this layer. */
		endcomps = &tile->tcmpts[tile->numtcmpts];
		for (comp = tile->tcmpts; comp != endcomps; ++comp) {
			endlvls = &comp->rlvls[comp->numrlvls];
			for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
				if (!lvl->bands) {
					continue;
				}
				endbands = &lvl->bands[lvl->numbands];
				for (band = lvl->bands; band != endbands; ++band) {
					if (!band->data) {
						continue;
					}
					for (prcno = 0, prc = band->prcs; prcno < lvl->numprcs;
					  ++prcno, ++prc) {
						if (!prc->cblks) {
							continue;
						}
						endcblks = &prc->cblks[prc->numcblks];
						for (cblk = prc->cblks; cblk != endcblks; ++cblk) {
							if (cblk->curpass) {
								endpasses = &cblk->passes[cblk->numpasses];
								pass1 = cblk->curpass;
								if (success) {
									for (pass = cblk->curpass; pass !=
									  endpasses; ++pass) {
										if (pass->rdslope >= goodthresh) {
											pass1 = &pass[1];
										}
									}
								}
								for (pass = cblk->curpass; pass != pass1;
								  ++pass) {
									pass->lyrno = lyrno;
								}
								for (; pass != endpasses; ++pass) {
									pass->lyrno = -1;
								}
							}
						}
					}
				}
			}
		}

		/* Perform tier 2 coding. */
		endcomps = &tile->tcmpts[tile->numtcmpts];
		for (comp = tile->tcmpts; comp != endcomps; ++comp) {
			endlvls = &comp->rlvls[comp->numrlvls];
			for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
				if (!lvl->bands) {
					continue;
				}
				for (prcno = 0; prcno < lvl->numprcs; ++prcno) {
					if (jpc_enc_encpkt(enc, out, comp - tile->tcmpts,
					  lvl - comp->rlvls, prcno, lyrno)) {
						return -1;
					}
				}
			}
		}
	}

	if (jas_getdbglevel() >= 5) {
		dump_layeringinfo(enc);
	}

	jas_stream_close(out);

	JAS_DBGLOG(10, ("finished rate allocation\n"));

	return 0;
}