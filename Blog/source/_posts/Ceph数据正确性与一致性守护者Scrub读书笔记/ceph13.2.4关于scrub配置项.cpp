OPTION(mon_warn_not_scrubbed, OPT_INT)
OPTION(mon_warn_not_deep_scrubbed, OPT_INT)
OPTION(mon_scrub_interval, OPT_INT) // once a day
OPTION(mon_scrub_timeout, OPT_INT) // let's give it 5 minutes; why not.
OPTION(mon_scrub_max_keys, OPT_INT) // max number of keys to scrub each time
OPTION(mon_scrub_inject_crc_mismatch, OPT_DOUBLE) // probability of injected crc mismatch [0.0, 1.0]
OPTION(mon_scrub_inject_missing_keys, OPT_DOUBLE) // probability of injected missing keys [0.0, 1.0]
OPTION(mds_max_scrub_ops_in_progress, OPT_INT) // the number of simultaneous scrubs allowed

OPTION(osd_op_queue_mclock_scrub_res, OPT_DOUBLE)
OPTION(osd_op_queue_mclock_scrub_wgt, OPT_DOUBLE)
OPTION(osd_op_queue_mclock_scrub_lim, OPT_DOUBLE)
OPTION(osd_scrub_invalid_stats, OPT_BOOL)
OPTION(osd_max_scrubs, OPT_INT)					// 单个OSD最大并发scrub数，Maximum concurrent scrubs on a single OSD
OPTION(osd_scrub_during_recovery, OPT_BOOL)		// 当OSD上的PG正在recovery时允许scrub，Allow new scrubs to start while recovery is active on the OSD
OPTION(osd_scrub_begin_hour, OPT_INT)			// 限制scrubbing在每天几点开始
OPTION(osd_scrub_end_hour, OPT_INT)				// 限制scrubbing在每天几点结束
OPTION(osd_scrub_begin_week_day, OPT_INT)		// 限制scrubbing在一周中的哪天开始（星期几开始，0 or 7 = Sunday, 1 = Monday, etc.）
OPTION(osd_scrub_end_week_day, OPT_INT)			// 限制scrubbing在一周中的哪天结束（星期几结束，0 or 7 = Sunday, 1 = Monday, etc.）
OPTION(osd_scrub_load_threshold, OPT_FLOAT)		// 允许scrubbing低于：系统负载（system load）除以CPU数量的值
OPTION(osd_scrub_min_interval, OPT_FLOAT)		// 每隔一次PG Scrub频率不超过此间隔，if load is low（如果负载低）
OPTION(osd_scrub_max_interval, OPT_FLOAT)		// 每隔一次PG Scrub频率不低于此间隔，regardless of load（无论负载如何）
OPTION(osd_scrub_interval_randomize_ratio, OPT_FLOAT)	// randomize the scheduled scrub in the span of [min,min*(1+randomize_ratio))
OPTION(osd_scrub_backoff_ratio, OPT_DOUBLE)		// scrub调度尝试失败后的退避率，the probability to back off the scheduled scrub
OPTION(osd_scrub_chunk_min, OPT_INT)			// 在单个chunk中scrub的最小对象数
OPTION(osd_scrub_chunk_max, OPT_INT)			// 在单个chunk中scrub的最大对象数
OPTION(osd_scrub_sleep, OPT_FLOAT)				// 持续scrub过程中注入延迟，sleep between [deep]scrub ops（在deep scrubbing操作之间sleep）
OPTION(osd_scrub_auto_repair, OPT_BOOL)			// 自动修复scrub过程中检测到的受损对象，whether auto-repair inconsistencies upon deep-scrubbing（是否在deep-scrubbing时自动修复不一致）
OPTION(osd_scrub_auto_repair_num_errors, OPT_U32)	// 自动修复的最大检测错误数，only auto-repair when number of errors is below this threshold（仅在错误数低于此阈值时自动修复）
OPTION(osd_deep_scrub_interval, OPT_FLOAT)		// Deep scrub每个PG（即，验证数据校验和），once a week（每周一次）
OPTION(osd_deep_scrub_randomize_ratio, OPT_FLOAT)	// Deep scrub间隔比例随机变化，scrubs will randomly become deep scrubs at this rate (0.15 -> 15% of scrubs are deep)
OPTION(osd_deep_scrub_stride, OPT_INT)			// 
OPTION(osd_deep_scrub_keys, OPT_INT)			// 
OPTION(osd_deep_scrub_update_digest_min_age, OPT_INT)   // objects must be this old (seconds) before we update the whole-object digest on scrub
OPTION(osd_deep_scrub_large_omap_object_key_threshold, OPT_U64)
OPTION(osd_deep_scrub_large_omap_object_value_sum_threshold, OPT_U64)
OPTION(osd_debug_deep_scrub_sleep, OPT_FLOAT)
OPTION(osd_scrub_priority, OPT_U32)
OPTION(osd_scrub_cost, OPT_U32) // set default cost equal to 50MB io
OPTION(osd_requested_scrub_priority, OPT_U32)	// set requested scrub priority higher than scrub priority to make the requested scrubs jump the queue of scheduled scrubs