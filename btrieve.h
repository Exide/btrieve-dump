#ifndef BTRIEVE_H
#define BTRIEVE_H

#include <libc.h>

enum Format { Old, New };

struct BtrieveFile {
    enum Format format;
    uint32_t page_count;
    uint16_t page_size;
    uint32_t record_count;
    int16_t physical_length;
    void* memory;
};

/**
 * Undoes Btrieve's word-swapping behaviour
 */
uint32_t swap_words(uint32_t n) {
    return ((n >> 16) & 0xFFFF) | (n << 16);
}

/**
 * Loads the file into contiguous memory and returns an object to interact with that block of memory.
 */
int load(FILE* file, struct BtrieveFile* btrieve_file) {

    {
        uint16_t exit_code = fseek(file, 0, SEEK_END);
        if (exit_code != 0) return 1;
    }

    size_t file_size = ftell(file);
    if (file_size == -1) return 1;

    btrieve_file->memory = malloc(file_size);

    {
        uint16_t exit_code = fseek(file, 0x00, SEEK_SET);
        if (exit_code != 0) return 1;
    }
    {
        uint16_t items_requested = 1;
        uint16_t items_read = fread(btrieve_file->memory, file_size, items_requested, file);
        if (items_read < items_requested) return 1;
    }

    // read 2 bytes @ 0x00 - must be either 0x0000 (pre-6) or 0x4346 (6+)
    uint16_t format_bytes = *(uint16_t*)btrieve_file->memory;
    uint32_t is_old_format = format_bytes == 0x0000;
    uint32_t is_new_format = format_bytes == 0x4346;
    btrieve_file->format = is_old_format ? Old : New;
    if (!is_old_format && !is_new_format) return 1;

    // read 2 bytes @ 0x08 - page size
    btrieve_file->page_size = *(uint16_t*)((size_t)btrieve_file->memory + 0x08);

    void* fcr;

    if (is_old_format) {
        fcr = (void*)((size_t)btrieve_file->memory);
    } else {
        // get the offset to the first fcr
        void* first_fcr = (void*)((size_t)btrieve_file->memory);
        int32_t* first_fcr_usage = (int32_t*)((size_t)first_fcr + 0x04);

        // get the offset to the second fcr
        void* second_fcr = (void*)((size_t)btrieve_file->memory + btrieve_file->page_size);
        int32_t* second_fcr_usage = (int32_t*)((size_t)second_fcr + 0x04);

        // get the active fcr
        fcr = (first_fcr_usage > second_fcr_usage) ? first_fcr : second_fcr;
    }

    // read 2 bytes @ 0x18 - physical record length, including pointers
    btrieve_file->physical_length = *(int16_t*)((size_t)fcr + 0x18);

    // read 4 bytes @ 0x1A - number of records in file (word-swapped)
    btrieve_file->record_count = swap_words(*(int32_t*)((size_t)fcr + 0x1A));

    // read 4 bytes @ 0x26 - number of pages in file (word-swapped)
    btrieve_file->page_count = swap_words(*(int32_t*)((size_t)fcr + 0x26));

    return 0;
}

/**
 * Copies all of the record data contiguously in a buffer
 */
int get_records(struct BtrieveFile* btrieve_file, void* buffer) {
    size_t current_record_index = 0;
    size_t page_index_after_fcr = btrieve_file->format == Old ? 1 : 2;

    for (size_t page_index = page_index_after_fcr; page_index < btrieve_file->page_count; ++page_index) {

        size_t page_offset = btrieve_file->page_size * page_index;
        uint16_t page_usage = *(uint16_t*)((size_t)btrieve_file->memory + page_offset + 0x04);

        int is_data_page = page_usage & 0x8000;
        if (!is_data_page) continue;

        size_t page_header_size = 0x06;
        size_t usable_page_size = btrieve_file->page_size - page_header_size;
        size_t records_on_this_page = usable_page_size / btrieve_file->physical_length;

        for (int record_index_on_page = 0; record_index_on_page < records_on_this_page; record_index_on_page++) {

            if (current_record_index >= btrieve_file->record_count) {
                // Btrieve allocates the maximum number of records that will fit in a page. Stop once we've copied
                // all the known records (i.e. record_count).
                break;
            }

            size_t source_offset = page_offset + page_header_size + (record_index_on_page * btrieve_file->physical_length);
            void* source_address = (void*)((size_t)btrieve_file->memory + source_offset);

            size_t destination_offset = current_record_index * btrieve_file->physical_length;
            void* destination_address = (void*)((size_t)buffer + destination_offset);

            memcpy(destination_address, source_address, btrieve_file->physical_length);
            current_record_index++;
        }
    }

    return 0;
}

#endif //BTRIEVE_H
