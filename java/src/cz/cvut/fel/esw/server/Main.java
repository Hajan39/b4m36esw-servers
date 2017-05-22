package cz.cvut.fel.esw.server;

import org.rapidoid.buffer.Buf;
import org.rapidoid.env.Env;
import org.rapidoid.http.AbstractHttpServer;
import org.rapidoid.http.HttpStatus;
import org.rapidoid.http.MediaType;
import org.rapidoid.net.abstracts.Channel;
import org.rapidoid.net.impl.RapidoidHelper;

import java.io.*;

/**
 * The ESW Java Server.
 */
public class Main {

    private static final int PORT = 8321; // Port on which the server runs.
    private static Dictionary dictionary = new Dictionary(); // Dictionary instance

    static {
        System.out.println("Java Rapidoid Server, v1.0");
    }

    public static void main(String[] args) {
        Env.setProfiles("production");
        new WordCountingHttpServer().listen(PORT);
    }

    /**
     * Rapoidoid HTTP server implementation.
     */
    public static class WordCountingHttpServer extends AbstractHttpServer {

        private static final byte[] URI_POST = "/esw/myserver/data".getBytes();
        private static final byte[] URI_GET = "/esw/myserver/count".getBytes();
        private static final byte[] EMPTY = new byte[0];

        /**
         * Handles incoming connection.
         *
         * @param ctx NIO channel.
         * @param buf Buffer.
         * @param req Request data.
         * @return The HTTP status.
         */
        @Override
        protected HttpStatus handle(final Channel ctx, final Buf buf, final RapidoidHelper req) {
            if (matches(buf, req.path, URI_POST) && !req.isGet.value) {
                try {
                    dictionary.decompressAndParse(req.body.bytes(buf));
                } catch (IOException e) {
                    e.printStackTrace();
                }
                startResponse(ctx, 204, req.isKeepAlive.value);
                writeBody(ctx, EMPTY, MediaType.TEXT_PLAIN);
                return HttpStatus.DONE;

            } else if (matches(buf, req.path, URI_GET) && req.isGet.value) {
                String count = Integer.toString(dictionary.countAndReset());
                return ok(ctx, req.isKeepAlive.value, count.getBytes(), MediaType.TEXT_PLAIN);
            }

            return HttpStatus.NOT_FOUND;
        }

    }

}
