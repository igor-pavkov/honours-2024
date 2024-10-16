static int mxf_parse_structural_metadata(MXFContext *mxf)
{
    MXFPackage *material_package = NULL;
    MXFPackage *temp_package = NULL;
    int i, j, k, ret;

    av_dlog(mxf->fc, "metadata sets count %d\n", mxf->metadata_sets_count);
    /* TODO: handle multiple material packages (OP3x) */
    for (i = 0; i < mxf->packages_count; i++) {
        material_package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[i], MaterialPackage);
        if (material_package) break;
    }
    if (!material_package) {
        av_log(mxf->fc, AV_LOG_ERROR, "no material package found\n");
        return AVERROR_INVALIDDATA;
    }

    for (i = 0; i < material_package->tracks_count; i++) {
        MXFPackage *source_package = NULL;
        MXFTrack *material_track = NULL;
        MXFTrack *source_track = NULL;
        MXFTrack *temp_track = NULL;
        MXFDescriptor *descriptor = NULL;
        MXFStructuralComponent *component = NULL;
        MXFTimecodeComponent *mxf_tc = NULL;
        UID *essence_container_ul = NULL;
        const MXFCodecUL *codec_ul = NULL;
        const MXFCodecUL *container_ul = NULL;
        const MXFCodecUL *pix_fmt_ul = NULL;
        AVStream *st;
        AVTimecode tc;
        int flags;

        if (!(material_track = mxf_resolve_strong_ref(mxf, &material_package->tracks_refs[i], Track))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track strong ref\n");
            continue;
        }

        if ((component = mxf_resolve_strong_ref(mxf, &material_track->sequence_ref, TimecodeComponent))) {
            mxf_tc = (MXFTimecodeComponent*)component;
            flags = mxf_tc->drop_frame == 1 ? AV_TIMECODE_FLAG_DROPFRAME : 0;
            if (av_timecode_init(&tc, mxf_tc->rate, flags, mxf_tc->start_frame, mxf->fc) == 0) {
                mxf_add_timecode_metadata(&mxf->fc->metadata, "timecode", &tc);
            }
        }

        if (!(material_track->sequence = mxf_resolve_strong_ref(mxf, &material_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track sequence strong ref\n");
            continue;
        }

        for (j = 0; j < material_track->sequence->structural_components_count; j++) {
            component = mxf_resolve_strong_ref(mxf, &material_track->sequence->structural_components_refs[j], TimecodeComponent);
            if (!component)
                continue;

            mxf_tc = (MXFTimecodeComponent*)component;
            flags = mxf_tc->drop_frame == 1 ? AV_TIMECODE_FLAG_DROPFRAME : 0;
            if (av_timecode_init(&tc, mxf_tc->rate, flags, mxf_tc->start_frame, mxf->fc) == 0) {
                mxf_add_timecode_metadata(&mxf->fc->metadata, "timecode", &tc);
                break;
            }
        }

        /* TODO: handle multiple source clips */
        for (j = 0; j < material_track->sequence->structural_components_count; j++) {
            component = mxf_resolve_strong_ref(mxf, &material_track->sequence->structural_components_refs[j], SourceClip);
            if (!component)
                continue;

            for (k = 0; k < mxf->packages_count; k++) {
                temp_package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[k], SourcePackage);
                if (!temp_package)
                    continue;
                if (!memcmp(temp_package->package_uid, component->source_package_uid, 16)) {
                    source_package = temp_package;
                    break;
                }
            }
            if (!source_package) {
                av_dlog(mxf->fc, "material track %d: no corresponding source package found\n", material_track->track_id);
                break;
            }
            for (k = 0; k < source_package->tracks_count; k++) {
                if (!(temp_track = mxf_resolve_strong_ref(mxf, &source_package->tracks_refs[k], Track))) {
                    av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track strong ref\n");
                    ret = AVERROR_INVALIDDATA;
                    goto fail_and_free;
                }
                if (temp_track->track_id == component->source_track_id) {
                    source_track = temp_track;
                    break;
                }
            }
            if (!source_track) {
                av_log(mxf->fc, AV_LOG_ERROR, "material track %d: no corresponding source track found\n", material_track->track_id);
                break;
            }
        }
        if (!source_track || !component)
            continue;

        if (!(source_track->sequence = mxf_resolve_strong_ref(mxf, &source_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track sequence strong ref\n");
            ret = AVERROR_INVALIDDATA;
            goto fail_and_free;
        }

        /* 0001GL00.MXF.A1.mxf_opatom.mxf has the same SourcePackageID as 0001GL.MXF.V1.mxf_opatom.mxf
         * This would result in both files appearing to have two streams. Work around this by sanity checking DataDefinition */
        if (memcmp(material_track->sequence->data_definition_ul, source_track->sequence->data_definition_ul, 16)) {
            av_log(mxf->fc, AV_LOG_ERROR, "material track %d: DataDefinition mismatch\n", material_track->track_id);
            continue;
        }

        st = avformat_new_stream(mxf->fc, NULL);
        if (!st) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not allocate stream\n");
            ret = AVERROR(ENOMEM);
            goto fail_and_free;
        }
        st->id = source_track->track_id;
        st->priv_data = source_track;
        source_track->original_duration = st->duration = component->duration;
        if (st->duration == -1)
            st->duration = AV_NOPTS_VALUE;
        st->start_time = component->start_position;
        if (material_track->edit_rate.num <= 0 ||
            material_track->edit_rate.den <= 0) {
            av_log(mxf->fc, AV_LOG_WARNING,
                   "Invalid edit rate (%d/%d) found on stream #%d, "
                   "defaulting to 25/1\n",
                   material_track->edit_rate.num,
                   material_track->edit_rate.den, st->index);
            material_track->edit_rate = (AVRational){25, 1};
        }
        avpriv_set_pts_info(st, 64, material_track->edit_rate.den, material_track->edit_rate.num);

        /* ensure SourceTrack EditRate == MaterialTrack EditRate since only
         * the former is accessible via st->priv_data */
        source_track->edit_rate = material_track->edit_rate;

        PRINT_KEY(mxf->fc, "data definition   ul", source_track->sequence->data_definition_ul);
        codec_ul = mxf_get_codec_ul(ff_mxf_data_definition_uls, &source_track->sequence->data_definition_ul);
        st->codec->codec_type = codec_ul->id;

        source_package->descriptor = mxf_resolve_strong_ref(mxf, &source_package->descriptor_ref, AnyType);
        if (source_package->descriptor) {
            if (source_package->descriptor->type == MultipleDescriptor) {
                for (j = 0; j < source_package->descriptor->sub_descriptors_count; j++) {
                    MXFDescriptor *sub_descriptor = mxf_resolve_strong_ref(mxf, &source_package->descriptor->sub_descriptors_refs[j], Descriptor);

                    if (!sub_descriptor) {
                        av_log(mxf->fc, AV_LOG_ERROR, "could not resolve sub descriptor strong ref\n");
                        continue;
                    }
                    if (sub_descriptor->linked_track_id == source_track->track_id) {
                        descriptor = sub_descriptor;
                        break;
                    }
                }
            } else if (source_package->descriptor->type == Descriptor)
                descriptor = source_package->descriptor;
        }
        if (!descriptor) {
            av_log(mxf->fc, AV_LOG_INFO, "source track %d: stream %d, no descriptor found\n", source_track->track_id, st->index);
            continue;
        }
        PRINT_KEY(mxf->fc, "essence codec     ul", descriptor->essence_codec_ul);
        PRINT_KEY(mxf->fc, "essence container ul", descriptor->essence_container_ul);
        essence_container_ul = &descriptor->essence_container_ul;
        /* HACK: replacing the original key with mxf_encrypted_essence_container
         * is not allowed according to s429-6, try to find correct information anyway */
        if (IS_KLV_KEY(essence_container_ul, mxf_encrypted_essence_container)) {
            av_log(mxf->fc, AV_LOG_INFO, "broken encrypted mxf file\n");
            for (k = 0; k < mxf->metadata_sets_count; k++) {
                MXFMetadataSet *metadata = mxf->metadata_sets[k];
                if (metadata->type == CryptoContext) {
                    essence_container_ul = &((MXFCryptoContext *)metadata)->source_container_ul;
                    break;
                }
            }
        }

        /* TODO: drop PictureEssenceCoding and SoundEssenceCompression, only check EssenceContainer */
        codec_ul = mxf_get_codec_ul(ff_mxf_codec_uls, &descriptor->essence_codec_ul);
        st->codec->codec_id = (enum AVCodecID)codec_ul->id;
        av_log(mxf->fc, AV_LOG_VERBOSE, "%s: Universal Label: ",
               avcodec_get_name(st->codec->codec_id));
        for (k = 0; k < 16; k++) {
            av_log(mxf->fc, AV_LOG_VERBOSE, "%.2x",
                   descriptor->essence_codec_ul[k]);
            if (!(k+1 & 19) || k == 5)
                av_log(mxf->fc, AV_LOG_VERBOSE, ".");
        }
        av_log(mxf->fc, AV_LOG_VERBOSE, "\n");

        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            source_track->intra_only = mxf_is_intra_only(descriptor);
            container_ul = mxf_get_codec_ul(mxf_picture_essence_container_uls, essence_container_ul);
            if (st->codec->codec_id == AV_CODEC_ID_NONE)
                st->codec->codec_id = container_ul->id;
            st->codec->width = descriptor->width;
            st->codec->height = descriptor->height; /* Field height, not frame height */
            switch (descriptor->frame_layout) {
                case SegmentedFrame:
                    /* This one is a weird layout I don't fully understand. */
                    av_log(mxf->fc, AV_LOG_INFO, "SegmentedFrame layout isn't currently supported\n");
                    break;
                case FullFrame:
                    st->codec->field_order = AV_FIELD_PROGRESSIVE;
                    break;
                case OneField:
                    /* Every other line is stored and needs to be duplicated. */
                    av_log(mxf->fc, AV_LOG_INFO, "OneField frame layout isn't currently supported\n");
                    break; /* The correct thing to do here is fall through, but by breaking we might be
                              able to decode some streams at half the vertical resolution, rather than not al all.
                              It's also for compatibility with the old behavior. */
                case MixedFields:
                    break;
                case SeparateFields:
                    switch (descriptor->field_dominance) {
                    case MXF_TFF:
                        st->codec->field_order = AV_FIELD_TT;
                        break;
                    case MXF_BFF:
                        st->codec->field_order = AV_FIELD_BB;
                        break;
                    default:
                        avpriv_request_sample(mxf->fc,
                                              "Field dominance %d support",
                                              descriptor->field_dominance);
                        break;
                    }
                    /* Turn field height into frame height. */
                    st->codec->height *= 2;
                    break;
                default:
                    av_log(mxf->fc, AV_LOG_INFO, "Unknown frame layout type: %d\n", descriptor->frame_layout);
            }
            if (st->codec->codec_id == AV_CODEC_ID_RAWVIDEO) {
                st->codec->pix_fmt = descriptor->pix_fmt;
                if (st->codec->pix_fmt == AV_PIX_FMT_NONE) {
                    pix_fmt_ul = mxf_get_codec_ul(ff_mxf_pixel_format_uls,
                                                  &descriptor->essence_codec_ul);
                    st->codec->pix_fmt = (enum AVPixelFormat)pix_fmt_ul->id;
                    if (st->codec->pix_fmt == AV_PIX_FMT_NONE) {
                        /* support files created before RP224v10 by defaulting to UYVY422
                           if subsampling is 4:2:2 and component depth is 8-bit */
                        if (descriptor->horiz_subsampling == 2 &&
                            descriptor->vert_subsampling == 1 &&
                            descriptor->component_depth == 8) {
                            st->codec->pix_fmt = AV_PIX_FMT_UYVY422;
                        }
                    }
                }
            }
            st->need_parsing = AVSTREAM_PARSE_HEADERS;
            if (material_track->sequence->origin) {
                av_dict_set_int(&st->metadata, "material_track_origin", material_track->sequence->origin, 0);
            }
            if (source_track->sequence->origin) {
                av_dict_set_int(&st->metadata, "source_track_origin", source_track->sequence->origin, 0);
            }
        } else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            container_ul = mxf_get_codec_ul(mxf_sound_essence_container_uls, essence_container_ul);
            /* Only overwrite existing codec ID if it is unset or A-law, which is the default according to SMPTE RP 224. */
            if (st->codec->codec_id == AV_CODEC_ID_NONE || (st->codec->codec_id == AV_CODEC_ID_PCM_ALAW && (enum AVCodecID)container_ul->id != AV_CODEC_ID_NONE))
                st->codec->codec_id = (enum AVCodecID)container_ul->id;
            st->codec->channels = descriptor->channels;
            st->codec->bits_per_coded_sample = descriptor->bits_per_sample;

            if (descriptor->sample_rate.den > 0) {
                st->codec->sample_rate = descriptor->sample_rate.num / descriptor->sample_rate.den;
                avpriv_set_pts_info(st, 64, descriptor->sample_rate.den, descriptor->sample_rate.num);
            } else {
                av_log(mxf->fc, AV_LOG_WARNING, "invalid sample rate (%d/%d) "
                       "found for stream #%d, time base forced to 1/48000\n",
                       descriptor->sample_rate.num, descriptor->sample_rate.den,
                       st->index);
                avpriv_set_pts_info(st, 64, 1, 48000);
            }

            /* if duration is set, rescale it from EditRate to SampleRate */
            if (st->duration != AV_NOPTS_VALUE)
                st->duration = av_rescale_q(st->duration,
                                            av_inv_q(material_track->edit_rate),
                                            st->time_base);

            /* TODO: implement AV_CODEC_ID_RAWAUDIO */
            if (st->codec->codec_id == AV_CODEC_ID_PCM_S16LE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S24LE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S32LE;
            } else if (st->codec->codec_id == AV_CODEC_ID_PCM_S16BE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S24BE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S32BE;
            } else if (st->codec->codec_id == AV_CODEC_ID_MP2) {
                st->need_parsing = AVSTREAM_PARSE_FULL;
            }
        } else if (st->codec->codec_type == AVMEDIA_TYPE_DATA) {
            int codec_id = mxf_get_codec_ul(mxf_data_essence_container_uls,
                                            essence_container_ul)->id;
            if (codec_id >= 0 &&
                codec_id < FF_ARRAY_ELEMS(mxf_data_essence_descriptor)) {
                av_dict_set(&st->metadata, "data_type",
                            mxf_data_essence_descriptor[codec_id], 0);
            }
        }
        if (descriptor->extradata) {
            if (!ff_alloc_extradata(st->codec, descriptor->extradata_size)) {
                memcpy(st->codec->extradata, descriptor->extradata, descriptor->extradata_size);
            }
        } else if (st->codec->codec_id == AV_CODEC_ID_H264) {
            ret = ff_generate_avci_extradata(st);
            if (ret < 0)
                return ret;
        }
        if (st->codec->codec_type != AVMEDIA_TYPE_DATA && (*essence_container_ul)[15] > 0x01) {
            /* TODO: decode timestamps */
            st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
        }
    }

    ret = 0;
fail_and_free:
    return ret;
}