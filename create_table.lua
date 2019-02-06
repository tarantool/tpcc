require('console').listen('unix/:./tarantool.sock')

box.cfg{
    pid_file   = "./tarantool-server.pid",
    log        = "./tarantool-server.log",
    listen = 3301,
    background = true,
    checkpoint_interval = 0,
    vinyl_memory =     512 * 1024 * 1024;
    vinyl_cache = 2 * 1024 * 1024 * 1024;
}

box.sql.execute("PRAGMA sql_default_engine='vinyl'")
box.sql.execute("drop table if exists warehouse;")
box.sql.execute("create table warehouse ( \
w_id int not null, \
w_name varchar(10), \
w_street_1 varchar(20), \
w_street_2 varchar(20), \
w_city varchar(20), \
w_state char(2), \
w_zip char(9), \
w_tax decimal(4,2), \
w_ytd decimal(12,2), \
primary key (w_id) )")

box.sql.execute("drop table if exists district;")
box.sql.execute("create table district ( \
d_id int not null, \
d_w_id int not null, \
d_name varchar(10), \
d_street_1 varchar(20), \
d_street_2 varchar(20), \
d_city varchar(20), \
d_state char(2), \
d_zip char(9), \
d_tax decimal(4,2), \
d_ytd decimal(12,2), \
d_next_o_id int, \
primary key (d_w_id, d_id), \
FOREIGN KEY(d_w_id) REFERENCES warehouse(w_id) );")

box.sql.execute("drop table if exists customer;")
box.sql.execute("create table customer ( \
c_id int not null, \
c_d_id int not null, \
c_w_id int not null, \
c_first varchar(16), \
c_middle char(2), \
c_last varchar(16), \
c_street_1 varchar(20), \
c_street_2 varchar(20), \
c_city varchar(20), \
c_state char(2), \
c_zip char(9), \
c_phone char(16), \
c_since varchar(100), \
c_credit char(2), \
c_credit_lim int, \
c_discount decimal(4,2), \
c_balance decimal(12,2), \
c_ytd_payment decimal(12,2), \
c_payment_cnt int, \
c_delivery_cnt int, \
c_data text, \
PRIMARY KEY(c_w_id, c_d_id, c_id), \
FOREIGN KEY(c_w_id,c_d_id) REFERENCES district(d_w_id,d_id) );")

box.sql.execute("drop table if exists history;")
box.sql.execute("create table history ( \
_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
h_c_id int, \
h_c_d_id int, \
h_c_w_id int, \
h_d_id int, \
h_w_id int, \
h_date varchar(100), \
h_amount decimal(6,2), \
h_data varchar(24), \
FOREIGN KEY(h_c_w_id,h_c_d_id,h_c_id) REFERENCES customer(c_w_id,c_d_id,c_id), \
FOREIGN KEY(h_w_id,h_d_id) REFERENCES district(d_w_id,d_id) );")

box.sql.execute("drop table if exists orders;")
box.sql.execute("create table orders ( \
o_id int not null, \
o_d_id int not null, \
o_w_id int not null, \
o_c_id int, \
o_entry_d varchar(100), \
o_carrier_id int, \
o_ol_cnt int, \
o_all_local int, \
PRIMARY KEY(o_w_id, o_d_id, o_id), \
FOREIGN KEY(o_w_id,o_d_id,o_c_id) REFERENCES customer(c_w_id,c_d_id,c_id) );")

box.sql.execute("drop table if exists new_orders;")
box.sql.execute("create table new_orders ( \
no_o_id int not null, \
no_d_id int not null, \
no_w_id int not null, \
PRIMARY KEY(no_w_id, no_d_id, no_o_id), \
FOREIGN KEY(no_w_id,no_d_id,no_o_id) REFERENCES orders(o_w_id,o_d_id,o_id));")

box.sql.execute("drop table if exists item;")
box.sql.execute("create table item ( \
i_id int not null, \
i_im_id int, \
i_name varchar(24), \
i_price decimal(5,2), \
i_data varchar(50), \
PRIMARY KEY(i_id) );")

box.sql.execute("drop table if exists stock;")
box.sql.execute("create table stock ( \
s_i_id int not null, \
s_w_id int not null, \
s_quantity int, \
s_dist_01 char(24), \
s_dist_02 char(24), \
s_dist_03 char(24), \
s_dist_04 char(24), \
s_dist_05 char(24), \
s_dist_06 char(24), \
s_dist_07 char(24), \
s_dist_08 char(24), \
s_dist_09 char(24), \
s_dist_10 char(24), \
s_ytd decimal(8,0), \
s_order_cnt int, \
s_remote_cnt int, \
s_data varchar(50), \
PRIMARY KEY(s_w_id, s_i_id), \
FOREIGN KEY(s_w_id) REFERENCES warehouse(w_id), \
FOREIGN KEY(s_i_id) REFERENCES item(i_id) );")

box.sql.execute("drop table if exists order_line;")
box.sql.execute("create table order_line ( \
ol_o_id int not null, \
ol_d_id int not null, \
ol_w_id int not null, \
ol_number int not null, \
ol_i_id int, \
ol_supply_w_id int, \
ol_delivery_d varchar(100), \
ol_quantity int, \
ol_amount decimal(6,2), \
ol_dist_info char(24), \
PRIMARY KEY(ol_w_id, ol_d_id, ol_o_id, ol_number), \
FOREIGN KEY(ol_w_id,ol_d_id,ol_o_id) REFERENCES orders(o_w_id,o_d_id,o_id), \
FOREIGN KEY(ol_supply_w_id,ol_i_id) REFERENCES stock(s_w_id,s_i_id) );")

box.sql.execute("CREATE INDEX idx_customer ON customer (c_w_id,c_d_id,c_last,c_first);")
box.sql.execute("CREATE INDEX idx_orders ON orders (o_w_id,o_d_id,o_c_id,o_id);")
box.sql.execute("CREATE INDEX fkey_stock_2 ON stock (s_i_id);")
box.sql.execute("CREATE INDEX fkey_order_line_2 ON order_line (ol_supply_w_id,ol_i_id);")

box.snapshot()
box.schema.user.grant('guest', 'super')
