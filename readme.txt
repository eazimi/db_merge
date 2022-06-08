Version 1.0

* Features
- Database dump -> converts *.db3 to *.sql // the dump file can be found in './dump_db1.sql' and './dump_db2.sql'
- Lists all the database tables
- Lists all the database table schemas
- Lists all the database table triggers
- Lists all the database table indices
- Lists all the tables which are in one database but not in the other one 
- Lists all the schema differences between the common tables 
- Lists all the trigger differences between the common tables

* How to run
1- go to the destination folder on your pc
2- git clone -b master git@code.siemens.com:kaco-sw/test-framework/common/db_tool.git
3- cd db_tool/
4- mkdir build
5- cd build
6- cmake ..
7- make
8- ./db_tool [PATH_TO_DB1] [PATH_TO_DB2] > out.sql (to save the logs in 'out.sql' in the current directory)

* Sample 
user-id@pc-id: ./db_tool ../db/config-app.db3 ../db/config-psd2.db3 > test.sql