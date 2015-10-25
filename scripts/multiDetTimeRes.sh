g++ -gdwarf-3 -c `root-config --cflags --glibs --libs` -std=c++11 multiDetTimeRes.cpp
g++ -gdwarf-3 -o multiDetTimeRes multiDetTimeRes.o `root-config --cflags --glibs --libs`
#./multiDetTimeRes
