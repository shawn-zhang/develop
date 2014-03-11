#!/bin/bash

ret=0
calculate_size()
{
    num=$1
    unit=${num:0-1:1}
    if [ $unit == 'G' ] || [ $unit == 'g' ]; then
        ret=$[ ${num:0:$[ ${#num} - 1 ]} * 1024 * 1024 * 1024 ]
    elif [ $unit == 'M' ] || [ $unit == 'm' ]; then
        ret=$[ ${num:0:$[ ${#num} - 1 ]} * 1024 * 1024 ]
    elif [ $unit == "K" ] || [ $unit == 'k' ]; then
        ret=$[ ${num:0:$[ ${#num} - 1 ]} * 1024 ]
    else
        ret=$[ ${num:0:${#num}} ]
    fi
}
if [ "$CFG_PART_SYSTEM_SIZE" -ne "" ]; then
echo "dengyukong"
CFG_PART_SYSTEM_SIZE=1600M
fi
CFG_NAND_BLOCK_SIZE=0x40000
CFG_NAND_PAGE_SIZE=4k

calculate_size $CFG_NAND_BLOCK_SIZE
nand_block_size=$ret
calculate_size $CFG_NAND_PAGE_SIZE
nand_page_size=$ret
nand_logic_block_size=$[ $nand_block_size - 2 * $nand_page_size ]
calculate_size $CFG_PART_SYSTEM_SIZE
part_system_size=$ret
max_block_count=$[ $part_system_size / $nand_block_size ]
reserve_block_count=$[ $max_block_count * 3 / 100 ]
echo $reserve_block_count
