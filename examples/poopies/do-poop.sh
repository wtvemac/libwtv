outELFFile=build/poopies.elf
sectionsDIR=../../sections
#base=0x8044004c

#objcopy --dump-section .text=$sectionsDIR/text.o $outELFFile
#objcopy --dump-section .data=$sectionsDIR/rodata.o $outELFFile
#objcopy --dump-section .data=$sectionsDIR/data.o $outELFFile
#headerSize=$(wc -c < "$sectionsDIR/header.o")
#textAddress=$(objdump -x -j .text $outELFFile | sed -En "s/^\s*[0-9]+\s+\.text\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
#rodataAddress=$(objdump -x -j .rodata $outELFFile | sed -En "s/^\s*[0-9]+\s+\.rodata\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
#dataAddress=$(objdump -x -j .data $outELFFile | sed -En "s/^\s*[0-9]+\s+\.data\s+[0-9a-f]+\s+([0-9a-f]+)\s+.*/\1/p")
#bssSize=$(objdump -x -j .bss $outELFFile | sed -En "s/^\s*[0-9]+\s+\.bss\s+([0-9a-f]+)\s+[0-9a-f]+\s+.*/\1/p")
#textSize=$((0x$rodataAddress - 0x$textAddress))
#cat $sectionsDIR/header.o $sectionsDIR/text.o > $sectionsDIR/out.o
#truncate -s $((headerSize + 30)) $sectionsDIR/out.o
#cat $sectionsDIR/rodata.o >> $sectionsDIR/out.o
#toDataSize=$(wc -c < "$sectionsDIR/out.o")
#cat $sectionsDIR/data.o >> $sectionsDIR/out.o
#dataAddress=$(((base - 0x4c) + toDataSize))
#dataSize=$(wc -c < "$sectionsDIR/data.o")
#truncate -s 2M $sectionsDIR/out.o
#perl -e "print pack('N', $dataAddress)" | dd of=$sectionsDIR/out.o bs=1 seek=24 count=4 conv=notrunc
#perl -e "print pack('N', $dataSize)" | dd of=$sectionsDIR/out.o bs=1 seek=28 count=4 conv=notrunc
#perl -e "print pack('N', 0x$bssSize)" | dd of=$sectionsDIR/out.o bs=1 seek=32 count=4 conv=notrunc

rm $sectionsDIR/out.o
objcopy --dump-section .code=$sectionsDIR/code.o $outELFFile
objcopy --dump-section .data=$sectionsDIR/data.o $outELFFile
cat $sectionsDIR/code.o $sectionsDIR/data.o > $sectionsDIR/out.o

python3 /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/rommy-1.0/rommy.py --fixcs $sectionsDIR/out.o