# https://github.com/lwip-tcpip/lwip
# MIDI: https://github.com/heyigor/miniBAE

outELFFile=wtv.o
outWTVHEADERfile=sections/header.o
base=0x8044004c
#base=0x9f00004c
#-Wl,-pie
#-Wl,--no-dynamic-linker
#-fdata-sections -ffunction-sections -Wl,--gc-sections
#-fno-rtti -nostdlib++ -nostdinc++
# -nostdinc
#-march=4650
mips-linux-gnu-gcc -O3 -G0 -nostdlib -nostdlib++ -ffreestanding -march=4650 -fno-exceptions -fno-pic -fno-plt -static -c main.cpp -o $outELFFile
mips-linux-gnu-gcc -O3 -G0 -nostdlib -nostdlib++ -ffreestanding -march=4650 -fno-exceptions -fno-pic -fno-plt -static -c crt0.s -o crt0.o
exit;
#-Wl,--entry=main -Wl,--section-start=.text=$base -Wall -Wextra -Wl,--section-start=.data=0x80001000

#dd if=$outELFFile bs=1 skip=65612 of=sections/all.o
#cat sections/header.o sections/all.o > sections/out.o

objcopy --dump-section .text=sections/text.o $outELFFile
objcopy --dump-section .rodata=sections/rodata.o $outELFFile
objcopy --dump-section .data=sections/data.o $outELFFile
headerSize=$(wc -c < "$outWTVHEADERfile")
textAddress=$(objdump -x -j .text $outELFFile | sed -En "s/^\s*[0-9]+\s+\.text\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
rodataAddress=$(objdump -x -j .rodata $outELFFile | sed -En "s/^\s*[0-9]+\s+\.rodata\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
dataAddress=$(objdump -x -j .data $outELFFile | sed -En "s/^\s*[0-9]+\s+\.data\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
bssSize=$(objdump -x -j .bss $outELFFile | sed -En "s/^\s*[0-9]+\s+\.bss\s+([0-9a-f]+)\s+[0-9a-f]+\s+.*/\1/p")
textSize=$((0x$rodataAddress - 0x$textAddress))
cat $outWTVHEADERfile sections/text.o > sections/out.o
truncate -s $((headerSize + textSize)) sections/out.o
cat sections/rodata.o >> sections/out.o
toDataSize=$(wc -c < "sections/out.o")
cat sections/data.o >> sections/out.o
dataAddress=$(((base - 0x4c) + toDataSize))
dataSize=$(wc -c < "sections/data.o")
truncate -s 2M sections/out.o
perl -e "print pack('N', $dataAddress)" | dd of=sections/out.o bs=1 seek=24 count=4 conv=notrunc
perl -e "print pack('N', $dataSize)" | dd of=sections/out.o bs=1 seek=28 count=4 conv=notrunc
perl -e "print pack('N', 0x$bssSize)" | dd of=sections/out.o bs=1 seek=32 count=4 conv=notrunc

python3 /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/rommy-1.0/rommy.py --fixcs /mnt/webtv-disk-tests/libwtv/sections/out.o

if [[ $base == '0x8044004c' ]]; then
exit
fi

#sudo dd if=/mnt/webtv-disk-tests/libwtv/sections/out.o conv=swab of=/dev/sda seek=17411
#sudo dd if=/mnt/webtv-disk-tests/libwtv/sections/out.o conv=swab of=/dev/sdb seek=17411

#sudo dd if=/mnt/webtv-disk-tests/write-approm.o conv=swab of=/dev/sda seek=17411
#sudo minicom -w -D /dev/ttyUSB0

python /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/mame-approm-it.py /mnt/webtv-disk-tests/libwtv/sections/out.o /mnt/WebTVHacking/Client/Tools/mame/nvram/wtv1sony/
cd /mnt/WebTVHacking/Client/Tools/mame/

#/mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -nodrc -debug wtv1sony
#/mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -nodrc wtv1sony

#/mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -debug wtv1sony
/mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse wtv1sony

cd /mnt/webtv-disk-tests/libwtv/


#clear; sudo rm -rf /opt/libwtv; make clobber; LIBWTV_INSTALL_DIR=/opt/libwtv ./build.sh

#rm ../../sections/out.o; clear; make clean; make; ls -lat .; ls -lat build/

#clear; sudo rm -rf /opt/libwtv; cd ../../; make clobber; LIBWTV_INSTALL_DIR=/opt/libwtv ./build.sh; cd examples/poopies; rm ../../sections/out.o; make clean; make; ls -lat .; ls -lat build/

###############

python /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/mame-approm-it.py /mnt/WebTVHacking/Client/Hacking/flashromimages/builds/bf0app/app/nondebug-production/build-7181/_approm.o /mnt/WebTVHacking/Client/Tools/mame/nvram/wtv1sony/; cd /mnt/WebTVHacking/Client/Tools/mame/; /mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -debug wtv1sony; cd /mnt/webtv-disk-tests/libwtv/examples/poopies/;

clear; cd /mnt/webtv-disk-tests/libwtv/examples/poopies; sudo rm -rf /opt/libwtv; cd ../../; make clobber; LIBWTV_INSTALL_DIR=/opt/libwtv WTV_INSTALL_DIR=/opt/libwtv ./build.sh; cd examples/poopies; rm ../../sections/out.o; make clean; make; ls -lat .; ls -lat build/

python /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/mame-approm-it.py /mnt/webtv-disk-tests/libwtv/sections/out.o /mnt/WebTVHacking/Client/Tools/mame/nvram/wtv1sony/; cd /mnt/WebTVHacking/Client/Tools/mame/; /mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -debug wtv1sony; cd /mnt/webtv-disk-tests/libwtv/examples/poopies/;


