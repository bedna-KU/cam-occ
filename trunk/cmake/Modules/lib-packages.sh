#!/bin/sh

#generate package and version list from a binary.
#need to integrate into CMake/CPack

echo -n This will take a minute:  >&2
#find libs: 
libs=`ldd bin/camocc2 |sed -n 's/^.lib/lib/;s/\ .*$//;s/^lib/&/p'|sort|uniq`

#find packages
for i in $libs
do 
  #echo $i
  echo -n . >&2
  packages="$packages\n`apt-file search $i|grep -v -e lib32 -e -dbg -e -dev|head -n1|cut -d: -f1`"
done

#sort and find versions
echo >&2
echo Sorting and finding versions... >&2
supackages=`echo $packages|sort|uniq`
#echo $supackages

for package in $supackages
do
  echo $package \(\>\= `apt-cache show $package |grep Version|cut -d\  -f2|head -n1`\)
done


