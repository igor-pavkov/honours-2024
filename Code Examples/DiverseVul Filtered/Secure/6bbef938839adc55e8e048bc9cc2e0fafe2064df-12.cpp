static void mpeg4_encode_gop_header(MpegEncContext *s)
{
    int64_t hours, minutes, seconds;
    int64_t time;

    put_bits(&s->pb, 16, 0);
    put_bits(&s->pb, 16, GOP_STARTCODE);

    time = s->current_picture_ptr->f->pts;
    if (s->reordered_input_picture[1])
        time = FFMIN(time, s->reordered_input_picture[1]->f->pts);
    time = time * s->avctx->time_base.num;
    s->last_time_base = FFUDIV(time, s->avctx->time_base.den);

    seconds = FFUDIV(time, s->avctx->time_base.den);
    minutes = FFUDIV(seconds, 60); seconds = FFUMOD(seconds, 60);
    hours   = FFUDIV(minutes, 60); minutes = FFUMOD(minutes, 60);
    hours   = FFUMOD(hours  , 24);

    put_bits(&s->pb, 5, hours);
    put_bits(&s->pb, 6, minutes);
    put_bits(&s->pb, 1, 1);
    put_bits(&s->pb, 6, seconds);

    put_bits(&s->pb, 1, !!(s->avctx->flags & AV_CODEC_FLAG_CLOSED_GOP));
    put_bits(&s->pb, 1, 0);  // broken link == NO

    ff_mpeg4_stuffing(&s->pb);
}