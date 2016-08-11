//=======================================================================
// Copyright Baptiste Wicht 2013-2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <file.hpp>
#include <system.hpp>
#include <errors.hpp>
#include <io.hpp>
#include <print.hpp>
#include <math.hpp>

#include "fat32_specs.hpp"

static constexpr const size_t BUFFER_SIZE = 4096;

int main(int argc, char* argv[]){
    if(argc < 3){
        printf("usage: mkfs fs device \n");

        exit(1);
    }

    auto fs_str = argv[1];
    auto device_str = argv[2];

    std::string fs(fs_str);

    if(fs == "fat32"){
        uint64_t size = 0;
        auto code = ioctl(device_str, ioctl_request::GET_BLK_SIZE, &size);

        if(code){
            printf("mkfs: error: %s\n", std::error_message(code));
            exit(1);
        }

        printf("mkfs: Creating Fat32 filesystem on %s\n", device_str);
        printf("mkfs: Device size: %m\n", size);

        uint64_t sector_size = 512;
        uint64_t sectors_per_cluster = 8;
        uint64_t sectors = size / sector_size;

        uint64_t available_sectors = sectors - sectors_per_cluster;
        uint64_t available_clusters = available_sectors / sectors_per_cluster;

        // Compute the size of the FAT
        uint64_t fat_size_bytes = available_clusters * sizeof(uint32_t);
        uint64_t fat_size_sectors = std::ceil_divide(fat_size_bytes, sector_size);
        uint64_t fat_size_clusters = std::ceil_divide(fat_size_sectors, sectors_per_cluster);
        fat_size_sectors = fat_size_clusters * sectors_per_cluster;

        printf("mkfs: Device sectors : %u\n", sectors);
        printf("mkfs: Available sectors : %u\n", available_sectors);
        printf("mkfs: FAT sectors : %u\n", fat_size_sectors);

        uint64_t free_clusters = available_clusters - fat_size_clusters - 1;
        uint64_t free_sectors = free_clusters * sectors_per_cluster;

        printf("mkfs: Free sectors : %u\n", free_sectors);
        printf("mkfs: Free clusters : %u\n", free_clusters);

        auto* fat_bs = new fat32::fat_bs_t();

        fat_bs->bytes_per_sector = sector_size;
        fat_bs->sectors_per_cluster = sectors_per_cluster;
        fat_bs->reserved_sectors = sectors_per_cluster - 2;
        fat_bs->number_of_fat = 1;
        fat_bs->root_directories_entries = 0;
        fat_bs->total_sectors = 0;
        fat_bs->total_sectors_long = sectors;
        fat_bs->sectors_per_fat = 0;
        fat_bs->sectors_per_fat_long = fat_size_sectors;
        fat_bs->root_directory_cluster_start = 2;
        fat_bs->fs_information_sector = 1;
        std::copy_n(&fat_bs->file_system_type[0], "FAT32", 5);
        fat_bs->signature = 0xAA55;

        // TODO Write fat_bs to write

        auto* fat_is = new fat32::fat_is_t();

        fat_is->allocated_clusters = 1;
        fat_is->free_clusters = free_clusters;
        fat_is->signature_start = 0x52526141;
        fat_is->signature_middle = 0x72724161;
        fat_is->signature_end = 0x000055AA;

        // TODO Write fat_bs

        //TODO Write FAT

        //TODO Write Root Cluster

        delete fat_is;
        delete fat_bs;

        exit(0);
    }

    printf("mkfs: Unsupported filesystem %s\n", fs_str);

    exit(0);
}
