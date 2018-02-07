#include "decrypt.h"
#include "encrypt.h"

uint8_t* decrypt_cipher(uint8_t* text, uint8_t* w, uint8_t Nr) {
    uint8_t* state = text;
    uint8_t round_key[16];
    memcpy(round_key, w + (Nr) * 16, 16);
    printf("round %d\n", 10);
    print_hex(round_key, 16);
    add_round_key(state, round_key);
    for(int i = Nr - 1; i > 0; i--) {
        inv_shift_rows(state);
        inv_sub_bytes(state);

        printf("round %d\n", i);
        memcpy(round_key, w + i * 16, 16);
        print_hex(round_key, 16);

        add_round_key(state, round_key);
        inv_mix_columns(state);
    }

    inv_shift_rows(state);
    inv_sub_bytes(state);
    memcpy(round_key, w, 16);
    printf("round %d\n", 0);
    print_hex(round_key, 16);;
    add_round_key(state, round_key);
    return state;
}

int inv_shift_rows(uint8_t* state) {
    //2nd row
    uint8_t temp = state[1];  //there are complex ways to use fewer temps, but the overhead is miniscule
    uint8_t temp2 = state[5];
    uint8_t temp3 = state[9];
    state[1] = state[13];
    state[5] = temp;
    state[9] = temp2;
    state[13] = temp3;

    //3rd row
    temp = state[2];
    temp2 = state[6];
    for(int i = 2; i < 8; i +=4) {
        state[i] = state[(8 + i) % 16];
    }
    state[10] = temp; 
    state[14] = temp2;
    //4th row
    temp = state[3];
    for(int i = 3; i < 12; i +=4) {
        state[i] = state[(i + 4) % 16];
    }
    state[15] = temp;
    return 1;
}

int inv_sub_bytes(uint8_t* state) {
    for(int i = 0; i < 16; i++) {
        state[i] = rev_sbox[state[i]];
    }
    return 1;
}

int inv_mix_columns(uint8_t* state) {
    uint8_t word [4];
    //col 1
    memcpy(word, state, 4);
    inv_mix_column_op(word);
    memcpy(state, word, 4);
    // print_hex(state, 4);

    //col2
    // for(int i = 1; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 4, 4);
    inv_mix_column_op(word);
    memcpy(state + 4, word, 4);
    //col3
    // for(int i = 2; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 8, 4);
    inv_mix_column_op(word);
    memcpy(state + 8, word, 4);
    //col4
    // for(int i = 3; i < 16; i += 4) {
    //     word[i/4] = state[i];
    // }
    memcpy(word, state + 12, 4);
    // print_hex(word, 4);
    inv_mix_column_op(word);
    memcpy(state + 12, word, 4);
    return 1;
}

int inv_mix_column_op(uint8_t* word) {
    uint8_t new_word[4];
    new_word[0] = mul_e[word[0]] ^ mul_b[word[1]] ^ mul_d[word[2]] ^ mul_9[word[3]];
    new_word[1] = mul_9[word[0]] ^ mul_e[word[1]] ^ mul_b[word[2]] ^ mul_d[word[3]];
    new_word[2] = mul_d[word[0]] ^ mul_9[word[1]] ^ mul_e[word[2]] ^ mul_b[word[3]];
    new_word[3] = mul_b[word[0]] ^ mul_d[word[1]] ^ mul_9[word[2]] ^ mul_e[word[3]];
    for(int i = 0; i < 4; i++) {
        word[i] = new_word[i];
    }
    return 1;
}