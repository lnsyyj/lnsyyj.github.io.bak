---
title: ceph pg 状态
date: 2020-03-27 15:17:30
tags: ceph
---

ceph 14.2.7

```
/*
 * pg states
 */
#define PG_STATE_CREATING           (1ULL << 0)  // creating
#define PG_STATE_ACTIVE             (1ULL << 1)  // i am active.  (primary: replicas too)
#define PG_STATE_CLEAN              (1ULL << 2)  // peers are complete, clean of stray replicas.
#define PG_STATE_DOWN               (1ULL << 4)  // a needed replica is down, PG offline
#define PG_STATE_RECOVERY_UNFOUND   (1ULL << 5)  // recovery stopped due to unfound
#define PG_STATE_BACKFILL_UNFOUND   (1ULL << 6)  // backfill stopped due to unfound
#define PG_STATE_PREMERGE           (1ULL << 7)  // i am prepare to merging
#define PG_STATE_SCRUBBING          (1ULL << 8)  // scrubbing
//#define PG_STATE_SCRUBQ           (1ULL << 9)  // queued for scrub
#define PG_STATE_DEGRADED           (1ULL << 10) // pg contains objects with reduced redundancy
#define PG_STATE_INCONSISTENT       (1ULL << 11) // pg replicas are inconsistent (but shouldn't be)
#define PG_STATE_PEERING            (1ULL << 12) // pg is (re)peering
#define PG_STATE_REPAIR             (1ULL << 13) // pg should repair on next scrub
#define PG_STATE_RECOVERING         (1ULL << 14) // pg is recovering/migrating objects
#define PG_STATE_BACKFILL_WAIT      (1ULL << 15) // [active] reserving backfill
#define PG_STATE_INCOMPLETE         (1ULL << 16) // incomplete content, peering failed.
#define PG_STATE_STALE              (1ULL << 17) // our state for this pg is stale, unknown.
#define PG_STATE_REMAPPED           (1ULL << 18) // pg is explicitly remapped to different OSDs than CRUSH
#define PG_STATE_DEEP_SCRUB         (1ULL << 19) // deep scrub: check CRC32 on files
#define PG_STATE_BACKFILLING        (1ULL << 20) // [active] backfilling pg content
#define PG_STATE_BACKFILL_TOOFULL   (1ULL << 21) // backfill can't proceed: too full
#define PG_STATE_RECOVERY_WAIT      (1ULL << 22) // waiting for recovery reservations
#define PG_STATE_UNDERSIZED         (1ULL << 23) // pg acting < pool size
#define PG_STATE_ACTIVATING         (1ULL << 24) // pg is peered but not yet active
#define PG_STATE_PEERED             (1ULL << 25) // peered, cannot go active, can recover
#define PG_STATE_SNAPTRIM           (1ULL << 26) // trimming snaps
#define PG_STATE_SNAPTRIM_WAIT      (1ULL << 27) // queued to trim snaps
#define PG_STATE_RECOVERY_TOOFULL   (1ULL << 28) // recovery can't proceed: too full
#define PG_STATE_SNAPTRIM_ERROR     (1ULL << 29) // error stopped trimming snaps
#define PG_STATE_FORCED_RECOVERY    (1ULL << 30) // force recovery of this pg before any other
#define PG_STATE_FORCED_BACKFILL    (1ULL << 31) // force backfill of this pg before any other
#define PG_STATE_FAILED_REPAIR      (1ULL << 32) // A repair failed to fix all errors
```

