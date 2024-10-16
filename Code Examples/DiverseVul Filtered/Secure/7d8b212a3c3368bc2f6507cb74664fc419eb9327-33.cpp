const char *RtmpProtocol::onSearchPacketTail(const char *data,size_t len){
    //移动拷贝提高性能
    auto next_step_func(std::move(_next_step_func));
    //执行下一步
    auto ret = next_step_func(data, len);
    if (!_next_step_func) {
        //为设置下一步，恢复之
        next_step_func.swap(_next_step_func);
    }
    return ret;
}