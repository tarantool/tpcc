local console = require('console')
console.listen("/tmp/tpcc-server.sock")

box.cfg {
    pid_file   = "./tpcc-server.pid",
    log        = "./tpcc-server.log",
    memtx_dir  = "/tmp/tpcc",
    listen = 3301,
    memtx_memory = 2000000000,
    background = true,
    checkpoint_interval = 0,
    wal_mode = 'none',
}


function try(f, catch_f)
    local status, exception = pcall(f)
    if not status then
        catch_f(exception)
    end
end

try(function()
    box.schema.user.grant('guest', 'read,write,execute', 'universe')
end, function(e)
    print(e)
end)