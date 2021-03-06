clear

source /afs/cern.ch/sw/lcg/contrib/gcc/4.8/x86_64-slc6/setup.csh
#source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.2/x86_64-slc6/setup.csh

setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/6.00.02/x86_64-slc6-gcc48-opt/root
#setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc49-opt/root

setenv PATH ${PATH}:$ROOTSYS/bin
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$ROOTSYS/lib64
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$ROOTSYS/lib
#setenv LD_LIBRARY_PATH $ROOTSYS/lib64

alias root $ROOTSYS/bin/root
alias root-config $ROOTSYS/bin/root-config
alias rootcint $ROOTSYS/bin/rootcint
alias rootcling $ROOTSYS/bin/rootcling

cd ..

which root
