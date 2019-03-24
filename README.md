## Quickstart

1. Install build dependencies: msgpack, tarantool-c.
2. Build tpcc: `cd src; make`.
3. Run tarantool instance: `tarantool create_table.lua`.
4. Load data: `tpcc_load -w1000` (see other options below).
5. Start workload: `./tpcc_start -w1000 -r10 -l10800`.

## Options

* `-w` warehouses;
* `-r` warmup time;
* `-l` benchmark time;
* `-i` report interval.

See `./tpcc_load --help` and `./tpcc_start --help` for more information.

## Output

With the defined interval (-i option), the tool will produce the following output:

```
  10, trx: 12920, 95%: 9.483, 99%: 18.738, max_rt: 213.169, 12919|98.778, 1292|101.096, 1293|443.955, 1293|670.842
  20, trx: 12666, 95%: 7.074, 99%: 15.578, max_rt: 53.733, 12668|50.420, 1267|35.846, 1266|58.292, 1267|37.421
  30, trx: 13269, 95%: 6.806, 99%: 13.126, max_rt: 41.425, 13267|27.968, 1327|32.242, 1327|40.529, 1327|29.580
  40, trx: 12721, 95%: 7.265, 99%: 15.223, max_rt: 60.368, 12721|42.837, 1271|34.567, 1272|64.284, 1272|22.947
  50, trx: 12573, 95%: 7.185, 99%: 14.624, max_rt: 48.607, 12573|45.345, 1258|41.104, 1258|54.022, 1257|26.626
```

Where: 

* 10 - the seconds from the start of the benchmark.
* trx: 12920 - New Order transactions executed during the gived interval (in
  this case, for the previous 10 sec). Basically this is the throughput per
  interval. More is better.
* 95%: 9.483: - The 95% Response time of New Order transactions per given
  interval. In this case it is 9.483 sec.
* 99%: 18.738: - The 99% Response time of New Order transactions per given
  interval. In this case it is 18.738 sec.
* max_rt: 213.169: - The Max Response time of New Order transactions per given
  interval. In this case it is 213.169 sec.
* the rest: `12919|98.778, 1292|101.096, 1293|443.955, 1293|670.842` is
  throughput and max response time for the other kind of transactions and can
  be ignored.

## Simple harness

```sh
TPCC_SRC_DIR="$(realpath .)"
TARANTOOL_SRC_DIR=<...>
TPCC_RESULTS_DIR=<...>
cp create_table.lua "${TARANTOOL_SRC_DIR}"

for w in 1 2 3 4 5 10 15; do
    cd "${TARANTOOL_SRC_DIR}"
    rm 00000*
    ./src/tarantool create_table.lua &
    sleep 5
    cd "${TPCC_SRC_DIR}"
    ./tpcc_load -w${w}

    for c in 10; do
        for i in $(seq 1 3); do
            ./tpcc_start -w${w} -r10 -l30 -i30 -c${c} >> "${TPCC_RESULTS_DIR}/tarantool-res-w${w}-c${c}.txt"
        done
    done

    killall tarantool
    wait
done
```
