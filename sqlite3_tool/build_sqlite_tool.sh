tar xzf sqlite.tar.gz
cd sqlite/         
mkdir bld                     
cd bld                 
../configure
make
cp sqlite3 /opt/ups/sqlite3           