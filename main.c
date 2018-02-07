#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "encrypt.h"
#include "decrypt.h"

//credit: https://en.wikipedia.org/wiki/Rijndael_key_schedule
uint8_t Rcon[256] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d
};

int convert_hex_to_bytes(char* hex_str, uint8_t* byte_array, int size) {
    //credit https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c
    char* ptr = hex_str;
    for (size_t count = 0; count < size; count++) {
        sscanf(ptr, "%2hhx", &byte_array[count]);
        ptr += 2;
    }
    return 1;
}

int key_expansion(uint8_t* key, uint16_t key_len, uint8_t* w, uint8_t num_of_rounds) {
    uint8_t Nk = key_len / 4; //512 / 4 < 256 ==> this is a safe op. Storing this val is faster, uses another byte of mem.
    uint8_t temp[4];
    memcpy(w, key, key_len); //I don't split the key into 4 byte words here, just deal with that as a flattened array for efficiency's sake
    for(int i = Nk; i < 4 * (num_of_rounds + 1); i++) {
        memcpy(temp, &w[4*(i-1)], 4); //copy the last word in w into temp
        // printf("original temp\n");
        // print_hex(temp, 4);
        if((i % Nk ) == 0) { //
            rot_word(temp);
            // printf("rotword temp\n");
            // print_hex(temp, 4);

            sub_word(temp);
            // printf("subword temp\n");

            // print_hex(temp, 4);

            temp[0] ^= Rcon[i / Nk];
            // printf("xored temp\n");

            // print_hex(temp, 4);
        } else if (Nk > 6 && i % Nk == 4) {
            sub_word(temp);
            // printf("subword temp\n");

            // print_hex(temp, 4);

        }
        for(int j = 0; j < 4; j++) {
            w[i*4 + j] = w[(i-Nk)*4 + j] ^ temp[j];
        }
        // printf("w at i - Nk\n");
        // print_hex(&w[(i-Nk*4)], 4);

        // printf("final w at i\n");
        // print_hex(&w[i*4],4);

    }
    return 1;
}

int main(int argc, char** argv) {
    if(argc != 4) {
        puts("Error: invalid arguments.  Proper format is: executable [e|d] [message] [key]");
        exit(0);
    }
    int encrypt; //used as boolean to denote encrypt or decrypt
    if(strlen(argv[1]) != 1 || (argv[1][0] != 'e' && argv[1][0] != 'd')) {
        puts("Error: invalid arg1.  First argument must be \'e\' or \'d\'");
        exit(0);
    } else {
        encrypt = argv[1][0] - 'd'; //if arg1 = d, then encrypt = 0 = false. else is true.
    }
    uint8_t input_text[16];     // 128 bits = 16 bytes
    if(strlen(argv[2]) != 32) {
        puts("Error: invalid arg2. Second argument must be 16 bytes in hexidecimal");
        exit(0);
    } else {
        //save the message into input_text
        convert_hex_to_bytes(argv[2], input_text, 16);
    }
    uint8_t* key; //store input key, size not fixed
    uint8_t key_len = strlen(argv[3]) / 2; //this is just set as uint8_t for efficiency, max allowed value is 32
    // printf("%u\n", key_len);
    uint8_t num_of_rounds;
    switch(key_len) {
    case 16: // 128 bits
        num_of_rounds = 10;
        break;
    case 24: //192 bits
        num_of_rounds = 12;
        break;
    case 32: //256 bits
        num_of_rounds = 14;
        break;
    default:
        printf("%s\n", "Invalid key size specified.  Key must be 16, 24, or 32 bytes in hexidecimal. Exiting.");
        exit(0);
    }
    key = malloc(key_len);

    convert_hex_to_bytes(argv[3], key, key_len);
    // print_hex(key, key_len);

    uint8_t* w = malloc(16*(num_of_rounds + 1));
    key_expansion(key, key_len, w, num_of_rounds);

    char* output;
    if(encrypt) {
        output = encrypt_cipher(input_text, w, num_of_rounds); //Technically less efficient to return ptr than pass and write into it, iirc
    } else {
        output = decrypt_cipher(input_text, w, num_of_rounds);
    }
    print_hex(output, 16);

    // printf("%s\n", output);

    return 0;
}