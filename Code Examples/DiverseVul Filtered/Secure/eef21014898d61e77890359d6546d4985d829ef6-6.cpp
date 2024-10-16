static void report_progress(const MYSQL *mysql __attribute__((unused)),
                            uint stage, uint max_stage,
                            double progress __attribute__((unused)),
                            const char *proc_info __attribute__((unused)),
                            uint proc_info_length __attribute__((unused)))
{
  progress_stage= stage;
  progress_max_stage= max_stage;
  progress_count++;
}