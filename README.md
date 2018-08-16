tiny memcached server. 
It supports get set and delete command from original protocol. You can find it here
https://github.com/memcached/memcached/blob/master/doc/protocol.txt

DEPENDENCIES:
- boost 1.67
- sqlite3

and as wrapper around sqlite3 I used sqlite_modern_cpp library. You can find it here, though
it is already included in project.

INSTALL
clone this repository, exctract and then:
mkdir build
cd build
cmake ..
make