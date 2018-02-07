#include "encrypt.h"

uint8_t* encrypt_cipher(uint8_t* text, uint8_t* w, uint8_t Nr) {
    uint8_t* state = text; //I flatten matrices into 1D arrays for efficiency. Equiv. to a 4x4 matrix of bytes
    uint8_t round_key[16];
    memcpy(round_key, w, 16);
    printf("round %d\n", 0);
    print_hex(round_key, 16);
    add_round_key(state, round_key);

    for(int i = 0; i < Nr - 1; i++) {
        sub_bytes(state);

        // printf("post sub\n");
        // print_hex(state, 16);

        shift_rows(state);

        // printf("post shift\n");
        // print_hex(state, 16);

        mix_columns(state);
        // printf("post mix\n");
        // print_hex(state, 16);

        printf("round %d\n", i+1);
        memcpy(round_key, w + (i + 1) * 16, 16);
        print_hex(round_key, 16);

        add_round_key(state, round_key);

        // printf("post rk\n");
        // print_hex(state, 16);
    }

    sub_bytes(state);
    shift_rows(state);
    memcpy(round_key, w + Nr * 16, 16);    
    printf("round %d\n", 10);
    print_hex(round_key, 16);

    add_round_key(state, round_key);

    return state;
}

int sub_word(uint8_t* word) {
    for(int i = 0; i < 4; i++) {
        word[i] = sbox[word[i]];
    }
    return 1;
}

int sub_bytes(uint8_t* state) {
    for(int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
    return 1;
}

int rot_word(uint8_t* word) {
    uint8_t temp = word[0];
    for(int i = 0; i < 3; i++) {
        word[i] = word[i + 1];
    }
    word[3] = temp;
    return 1;
}

int shift_rows(uint8_t* state) {
    //2nd row
    uint8_t temp = state[1];
    for(int i = 1; i < 12; i += 4) {
        state[i] = state[(4 + i) % 16];
    }
    state[13] = temp;

    //3rd row
    temp = state[2];
    uint8_t temp2 = state[6];
    for(int i = 2; i < 8; i += 4) {
        state[i] = state[(8 + i) % 16];
    }
    state[10] = temp;
    state[14] = temp2;

    //4th row
    temp = state[3];
    temp2 = state[7];
    uint8_t temp3 = state[11];
    state[3] = state[15];
    state[7] = temp;
    state[11] = temp2;
    state[15] = temp3;
    return 1;
}

int mix_columns(uint8_t* state) {
    uint8_t word [4];
    //col 1
    // for(int i = 0; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state, 4);
    mix_column_op(word);
    memcpy(state, word, 4);
    // print_hex(state, 4);

    //col2
    // for(int i = 1; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 4, 4);
    mix_column_op(word);
    memcpy(state + 4, word, 4);
    //col3
    // for(int i = 2; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 8, 4);
    mix_column_op(word);
    memcpy(state + 8, word, 4);
    //col4
    // for(int i = 3; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 12, 4);
    // print_hex(word, 4);
    mix_column_op(word);
    memcpy(state + 12, word, 4);
    return 1;
}

int mix_column_op(uint8_t* word) {
    uint8_t new_word[4];
    new_word[0] = mul_2[word[0]] ^ mul_3[word[1]] ^ word[2] ^ word[3];
    new_word[1] = word[0] ^ mul_2[word[1]] ^ mul_3[word[2]] ^ word[3];
    new_word[2] = word[0] ^ word[1] ^ mul_2[word[2]] ^ mul_3[word[3]];
    new_word[3] = mul_3[word[0]] ^ word[1] ^ word[2] ^ mul_2[word[3]];
    for(int i = 0; i < 4; i++) {
        word[i] = new_word[i];
    }
    return 1;
}

//this doesn't work
uint8_t multiply(uint8_t a, uint8_t b) {
    //credit: https://en.wikipedia.org/wiki/Rijndael_key_schedule

    uint16_t product = a * b;
    return product % 283; // 283 is equiv. to x^8 + x^4 + x^3 + x + 1
}

int add_round_key(uint8_t* state, uint8_t* round_key) {
    //col 1
    // for(int i = 0; i < 16; i += 4) {
    //     state[i] ^= round_key[i];
    // }
    // //col2
    // for(int i = 1; i < 16; i += 4) {
    //     state[i] ^= round_key[1 + i / 4];
    // }
    // //col3
    // for(int i = 2; i < 16; i += 4) {
    //     state[i] ^= round_key[2 + i / 4];
    // }
    // //col4
    // for(int i = 3; i < 16; i += 4) {
    //     state[i] ^= round_key[3 + i / 4];
    // }
    for(int i = 0; i < 16; i++) {
        state[i] ^= round_key[i];
    }
}

int print_hex(uint8_t* t, int size) {
    for(int i = 0; i < size; i++) {
        // if(t[i] < 0xa) {
        //     printf("0");
        // }
        printf("%02x", t[i]);
    }
    printf("\n");
    return 1;
}