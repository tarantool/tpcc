box.cfg {
    listen = 3301,
    memtx_memory = 4000000000,
}

box.schema.user.grant('guest', 'read,write', 'universe')
