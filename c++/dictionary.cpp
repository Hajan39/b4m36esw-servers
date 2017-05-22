#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <iostream>
#include <thread>
#include <pthread.h>
#include <string.h>
#include "dictionary.h"


/*
 * Built using this great tutorials.
 * https://www.tutorialspoint.com/cplusplus/cpp_multithreading.htm
 * https://panthema.net/2007/0328-ZLibString.html
 */

using namespace std;

std::unordered_set<std::string> word_set;
pthread_rwlock_t rw_lock;
pthread_rwlockattr_t rw_attr;
pthread_t threads[2000];
int i = 0;

/**
 * Initializes the dictionary.
 */
void init_dictionary() {
    pthread_rwlockattr_setkind_np(&rw_attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    pthread_rwlock_init(&rw_lock, &rw_attr);
}

/**
 * Adds word to the dictionary.
 *
 * @param word
 */
void add_word(std::string word) {
    pthread_rwlock_wrlock(&rw_lock);
    word_set.insert(word);
    pthread_rwlock_unlock(&rw_lock);
}

/**
 * Waits for all spawned threads to finish and then counts the dictionary size, resets the dictionary
 * and returns the size.
 *
 * @return Number of unique words.
 */
int count_unique_words() {
    void *status;
    for (int j = 0; j < i; j++) {
        pthread_join(threads[j], &status);
    }

    unsigned long unique_words = word_set.size();
    word_set.clear();
    i = 0;

    return unique_words;
}

/**
 * Decompresses and counts unique words in given POST data.
 *
 * @param data The decompressed POST data.
 */
void *count_unique_words_in_string(void *data) {
    std::string *str = (std::string *) data;

    char *saveptr;
    char sep[] = {' ', '\t', '\n', '\r'};
    char *token = strtok_r((char *) str->c_str(), sep, &saveptr);
    while (token) {
        std::string word(token);
        if (word_set.find(word) == word_set.end()) {
            add_word(word);
        }
        token = strtok_r(NULL, sep, &saveptr);
    }

    delete str;
    pthread_exit(NULL);
}

/**
 * Decompress POST data and add words to the dictionary.
 *
 * @param data The POST data.
 * @param len Length of the POST data.
 */
void decompress_and_parse(const char *data, const ssize_t len) {
    std::string *str_ptr = new std::string(decompress(data, len));

    int rc;
    pthread_t *thread = new pthread_t;
    pthread_attr_t attr;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    rc = pthread_create(thread, &attr, count_unique_words_in_string, (void *) str_ptr);
    if (rc) {
        std::cerr << "Error: unable to create thread: " << rc << endl;
        exit(-1);
    }
    threads[i++] = *thread;
    pthread_attr_destroy(&attr);
}

/**
 * Decompresses gzipped POST data.
 *
 * @param data The POST data.
 * @param len Length of the data.
 * @return Decompressed POST data.
 */
std::string decompress(const char *data, const ssize_t len) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 32 + MAX_WBITS) != Z_OK) {
        throw (std::runtime_error("inflateInit failed while decompressing."));
    }

    zs.next_in = (Bytef *) data;
    zs.avail_in = len;

    int ret;
    char outbuffer[OUT_BUF_SIZE];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw (std::runtime_error(oss.str()));
    }

    return outstring;
}
