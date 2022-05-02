Version 1.0

* Features
- Database dump -> converts *.db3 to *.sql // the dump file can be found in './dump_db1.sql' and './dump_db2.sql'
- Lists all the database tables
- Lists all the database table schemas
- Lists all the database table triggers
- Lists all the database table indices
- Lists all the tables differences between two input databases

* How to run
1- go to the destination folder on your pc
2- git clone -b master git@code.siemens.com:ehsan.azimi/db_merge.git
3- cd db_merge/
4- mkdir build
5- cd build
6- cmake ..
7- make
8- ./db_merge [PATH_TO_DB1] [PATH_TO_DB2] > out.sql (to save the logs in 'out.sql' in the current directory)

* Sample 
user-id@pc-id: ./db_merge ../db/config-app.db3 ../db/config-psd2.db3 > test.sql