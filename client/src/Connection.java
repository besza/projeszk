package src;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Connection {

    public interface Handler {
        public void message(String msg);
    }

    private BufferedReader input;
    private PrintWriter output;

    public void connect(String host, int port) throws IOException {
        Socket socket = new Socket(host, port);
        input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        output = new PrintWriter(socket.getOutputStream(), true);
    }

    public void send(String msg) {
        output.println(msg);
    }

    public void forwardInput(Handler handler) throws IOException {
        String line;
        while ((line = input.readLine()) != null) {
            handler.message(line);
        }
    }
}
