local console = require('console')
console.listen("/usr/local/var/run/tarantool/tpcc-server.control")

box.cfg {
    pid_file   = "/usr/local/var/run/tarantool/tpcc-server.pid",
    wal_dir    = "/usr/local/var/lib/tarantool/tpcc-server",
    memtx_dir  = "/usr/local/var/lib/tarantool/tpcc-server",
    log        = "/usr/local/var/log/tarantool/tpcc-server.log",
    username   = "root",
    listen = 3301,
    memtx_memory = 2000000000,
    checkpoint_interval = 0,
    background = true,
}
