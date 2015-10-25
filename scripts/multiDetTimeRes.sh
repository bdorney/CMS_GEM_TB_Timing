#g++ -gdwarf-3 -c `root-config --cflags --glibs --libs` -std=c++11 multiDetTimeRes.cpp
#g++ -gdwarf-3 -o multiDetTimeRes multiDetTimeRes.o `root-config --cflags --glibs --libs`
g++ -g3 -c -O0 `root-config --cflags --glibs --libs` -std=c++11 multiDetTimeRes.cpp -I../include
#g++ -g3 -O0 -shared multiDetTimeRes multiDetTimeRes.o `root-config --cflags --glibs --libs`
#g++ -g3 -O0 multiDetTimeRes multiDetTimeRes.o `root-config --cflags --glibs --libs` -I../include -fPIC
g++ -o multiDetTimeRes multiDetTimeRes.o  `root-config --cflags --glibs --libs`

#./multiDetTimeRes
