package cz.cvut.fel.esw.server;

import java.io.*;
import java.util.StringTokenizer;
import java.util.concurrent.ConcurrentHashMap;
import java.util.zip.GZIPInputStream;

/**
 * Singleton Dictionary, manages POST data parsing and word counting.
 */
class Dictionary {

    private static final Object PRESENT = new Object();
    private static final int GZIP_BUFFER_SIZE = 2048;
    private final ConcurrentHashMap<String, Object> dictionary = new ConcurrentHashMap<String, Object>(262144);

    /**
     * Returns number of words in dictionary and clears it.
     *
     * @return Number of unique words.
     */
    int countAndReset() {
        int val = dictionary.size();
        dictionary.clear();
        return val;
    }

    /**
     * Decompresses given bytes and then parses and adds unique words to the dictionary.
     *
     * @param bytes The bytes to unzip and parse
     * @throws IOException If something goes wrong.
     */
    void decompressAndParse(byte[] bytes) throws IOException {
        // Decompression
        BufferedReader reader =
                new BufferedReader(
                        new InputStreamReader(
                                new GZIPInputStream(
                                        new ByteArrayInputStream(bytes),
                                        GZIP_BUFFER_SIZE
                                )
                        ),
                        2 * bytes.length
                );

        // Word counting using parallel streams
        reader.lines().parallel().forEach(line -> {
            StringTokenizer st = new StringTokenizer(line);
            while (st.hasMoreTokens()) {
                String word = st.nextToken();
                if (!dictionary.containsKey(word)) {
                    dictionary.put(word, PRESENT);
                }
            }
        });
    }
}
