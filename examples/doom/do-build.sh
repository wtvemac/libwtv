#clear; cd /mnt/webtv-disk-tests/libwtv/examples/doom; sudo rm -rf /opt/libwtv; cd ../../; make clobber; LIBWTV_INSTALL_DIR=/opt/libwtv WTV_INSTALL_DIR=/opt/libwtv ./build.sh; cd examples/doom; rm ../../sections/out.o; make clean; make; ls -lat .; ls -lat build
clear; cd /mnt/webtv-disk-tests/libwtv/examples/doom; sudo rm -rf /opt/libwtv; cd ../../; LIBWTV_INSTALL_DIR=/opt/libwtv WTV_INSTALL_DIR=/opt/libwtv ./build.sh; cd examples/doom; rm ../../sections/out.o; make


if [ "$1" = "1" ]; then
		if test -f "/mnt/webtv-disk-tests/libwtv/sections/doom-out.o"; then
			if test -f "/mnt/webtv-disk-tests/libwtv/examples/doom/build/doom.elf"; then
				python /mnt/WebTVHacking/Client/Tools/romfs_tools/emac/lzj_release/mame-approm-it.py /mnt/webtv-disk-tests/libwtv/sections/doom-out.o /mnt/WebTVHacking/Client/Tools/mame/nvram/wtv1sony/; cd /mnt/WebTVHacking/Client/Tools/mame/; /mnt/WebTVHacking/Client/Tools/mame/wtvmame -window -verbose -skip_gameinfo -nomouse -debug wtv1sony; cd /mnt/webtv-disk-tests/libwtv/examples/doom/;
			else
				echo "No .elf file built... STOP"
			fi
		else
			echo "No .o file built... STOP"
		fi
else
	if [ "$1" = "2" ]; then
		if test -f "/mnt/webtv-disk-tests/libwtv/sections/doom-out.o"; then
			if test -f "/mnt/webtv-disk-tests/libwtv/examples/doom/build/doom.elf"; then
				read -p "Load in your SD card and press enter-->"
				sudo dd if=/mnt/webtv-disk-tests/libwtv/sections/doom-out.o conv=swab of=/dev/sda seek=17411
			else
				echo "No .elf file built... STOP"
			fi
		else
			echo "No .o file built... STOP"
		fi
	else
		if test -f "/mnt/webtv-disk-tests/libwtv/sections/doom-out.o"; then
			if test -f "/mnt/webtv-disk-tests/libwtv/examples/doom/build/doom.elf"; then
				echo "Stopped without doing anything... STOP (note: use '$0 1' to execute MAME, '$0 2' to copy to SD card)"
			else
				echo "No .elf file built... STOP"
			fi
		else
			echo "No .o file built... STOP"
		fi
	fi
fi