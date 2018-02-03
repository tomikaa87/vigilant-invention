otf2bdf.exe -r 72 -p 32 "c:\Users\tomikaa\Downloads\ultimate_oldschool_pc_font_pack_v1.0\PxPlus (TrueType - extended charset)\PxPlus_IBM_VGA8.ttf" -o pxplus-vga8-32.bdf

bdfconv.exe -v -f 1 -m "32,40-57" pxplus-vga8-32.bdf -o pxplus-vga8-32.c -n pxplus_vga8_32_nums -d pxplus-vga8-32.bdf