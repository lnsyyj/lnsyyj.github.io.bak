---
title: Ceph数据正确性与一致性守护者Scrub读书笔记
date: 2019-06-03 16:17:02
tags: Ceph
---

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/Ceph%E6%95%B0%E6%8D%AE%E6%AD%A3%E7%A1%AE%E6%80%A7%E4%B8%8E%E4%B8%80%E8%87%B4%E6%80%A7%E5%AE%88%E6%8A%A4%E8%80%85Scrub%E8%AF%BB%E4%B9%A6%E7%AC%94%E8%AE%B0/Scrub.png)



Ceph关于Scrub相关配置

```
OPTION(mon_warn_not_scrubbed, OPT_INT)					//
OPTION(mon_warn_not_deep_scrubbed, OPT_INT)				//
OPTION(mon_scrub_interval, OPT_INT)						// 默认值：一天一次
OPTION(mon_scrub_timeout, OPT_INT)						// 默认值：5分钟
OPTION(mon_scrub_max_keys, OPT_INT)						// 每次scrub的最大key数
OPTION(mon_scrub_inject_crc_mismatch, OPT_DOUBLE)		// 注入crc不匹配的概率[0.0,1.0]
OPTION(mon_scrub_inject_missing_keys, OPT_DOUBLE)		// 注入缺失key的概率[0.0,1.0]
OPTION(mds_max_scrub_ops_in_progress, OPT_INT)			// 并行执行的最大scrub操作数，允许同时scrub的数量

OPTION(osd_op_queue_mclock_scrub_res, OPT_DOUBLE)		// 
OPTION(osd_op_queue_mclock_scrub_wgt, OPT_DOUBLE)		// 
OPTION(osd_op_queue_mclock_scrub_lim, OPT_DOUBLE)		// 
OPTION(osd_scrub_invalid_stats, OPT_BOOL)				// 
OPTION(osd_max_scrubs, OPT_INT)							// 单个OSD最大并发scrub数
OPTION(osd_scrub_during_recovery, OPT_BOOL)				// 当OSD上的PG正在recovery时允许scrub
OPTION(osd_scrub_begin_hour, OPT_INT)					// 限制scrubbing在每天几点开始
OPTION(osd_scrub_end_hour, OPT_INT)						// 限制scrubbing在每天几点结束
OPTION(osd_scrub_begin_week_day, OPT_INT)				// 限制scrubbing在一周中的哪天开始（星期几开始，0 or 7 = Sunday, 1 = Monday, etc.）
OPTION(osd_scrub_end_week_day, OPT_INT)					// 限制scrubbing在一周中的哪天结束（星期几结束，0 or 7 = Sunday, 1 = Monday, etc.）
OPTION(osd_scrub_load_threshold, OPT_FLOAT)				// 允许scrubbing低于系统负载（system load）除以CPU数量的值
OPTION(osd_scrub_min_interval, OPT_FLOAT)				// 每隔一次PG Scrub频率不超过此间隔（如果负载低）
OPTION(osd_scrub_max_interval, OPT_FLOAT)				// 每隔一次PG Scrub频率不低于此间隔（无论负载如何）
OPTION(osd_scrub_interval_randomize_ratio, OPT_FLOAT)	// scrub间隔的比率随机变化，这可以通过随机改变scrub间隔来防止scrub 踩踏，以便它们很快在一周内均匀分布，在[min，min *（1 + randomize_ratio））的范围内随机化调度scrub
OPTION(osd_scrub_backoff_ratio, OPT_DOUBLE)				// scrub调度尝试失败后的退避率，the probability to back off the scheduled scrub
OPTION(osd_scrub_chunk_min, OPT_INT)					// 在单个chunk中scrub的最小对象数
OPTION(osd_scrub_chunk_max, OPT_INT)					// 在单个chunk中scrub的最大对象数
OPTION(osd_scrub_sleep, OPT_FLOAT)						// 持续scrub过程中注入延迟（在deep scrubbing操作之间sleep）
OPTION(osd_scrub_auto_repair, OPT_BOOL)					// 自动修复scrub过程中检测到的受损对象（是否在deep-scrubbing时自动修复不一致）
OPTION(osd_scrub_auto_repair_num_errors, OPT_U32)		// 自动修复的最大检测错误数（仅在错误数低于此阈值时自动修复）
OPTION(osd_deep_scrub_interval, OPT_FLOAT)				// Deep scrub每个PG（即，验证数据校验和）（每周一次）
OPTION(osd_deep_scrub_randomize_ratio, OPT_FLOAT)		// Deep scrub间隔比例随机变化，这可以通过随机改变scrub间隔来防止deep scrub 踩踏，当没有用户启动scrub时添加随机，scrub将随机变成这种速率的deep scrub（0.15  - > 15％ deep scrub）
OPTION(osd_deep_scrub_stride, OPT_INT)					// Deep scrub期间一次从对象读取的字节数
OPTION(osd_deep_scrub_keys, OPT_INT)									// Deep scrub期间一次从对象读取的key数
OPTION(osd_deep_scrub_update_digest_min_age, OPT_INT)					// 仅当对象的上次修改时间超过此时间时才更新整体对象digest（摘要）
OPTION(osd_deep_scrub_large_omap_object_key_threshold, OPT_U64)			// 当我们遇到某个对象的omap key多于此阈值时，发出警告
OPTION(osd_deep_scrub_large_omap_object_value_sum_threshold, OPT_U64)	// 当我们遇到某个对象的omap key bytes大小多于此阈值时，发出警告
OPTION(osd_debug_deep_scrub_sleep, OPT_FLOAT)			// 在deep scrub IO期间注入sleep以使其更容易induce preemption（诱导抢占）
OPTION(osd_scrub_priority, OPT_U32)						// 工作队列中的scrub操作的优先级
OPTION(osd_scrub_cost, OPT_U32)							// 工作队列中的scrub操作的cost（设置默认cost等于50MB io）（cost翻译为成本，需要看代码到底指的是什么）
OPTION(osd_requested_scrub_priority, OPT_U32)			// 设置请求的scrub优先级高于scrub优先级，使请求的scrub跳过预定scrub的队列
```



临时结论：

1、目前无法避免静默错误，Scrub也无法修复严重的静默错误

2、Scrub是Ceph检测静默错误的机制

3、目前发现静默错误也没有好的方法修复（只能通知客户尽可能挽回数据）



想法：

1、借助Scrub作为测试正确性与一致性的工具（需要解决覆盖写问题）

