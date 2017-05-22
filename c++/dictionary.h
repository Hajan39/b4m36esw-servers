#ifndef SERVER_DICTIONARY_H
#define SERVER_DICTIONARY_H

#include <vector>
#include <unordered_set>

#define OUT_BUF_SIZE 1048576
#define NUM_THREADS 24

void init_dictionary();
void add_word(std::string);
void *count_unique_words_in_string(void *);
std::string decompress(const char *, const ssize_t);
int count_unique_words();
void decompress_and_parse(const char *, const ssize_t);

#endif //SERVER_DICTIONARY_H
