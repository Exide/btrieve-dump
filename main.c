#include <stdio.h>
#include <stdlib.h>
#include "btrieve.h"

void print_usage() {
    fprintf(stdout, "Usage: btrieve-dump <BTRIEVE_FILE>\n");
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        print_usage();
        return 0;
    }

    if (argc != 2) {
        print_usage();
        fprintf(stderr, "error: invalid number of arguments\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "file not found\n");
        return 1;
    }

    struct BtrieveFile btrieve_file = {};
    if(load(file, &btrieve_file)) {
        fprintf(stderr, "unable to load Btrieve file\n");
        return 1;
    }

    fclose(file);

    void* records = malloc(btrieve_file.record_count * btrieve_file.physical_length);
    if(get_records(&btrieve_file, records)) {
        fprintf(stderr, "unable to get records from Btrieve file\n");
        return 1;
    }

    for (int record_index = 0; record_index < btrieve_file.record_count; ++record_index) {
        size_t record_offset = btrieve_file.physical_length * record_index;
        void* record_pointer = (void*)((size_t) records + record_offset);
        for (int i = 0; i < btrieve_file.physical_length; ++i) {
            printf("%02x ", ((unsigned char*)record_pointer)[i]);
        }
        putchar('\n');
    }

    return 0;
}
