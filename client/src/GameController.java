package src;

import java.io.IOException;

public class GameController {

    enum Piece {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN
    }

    enum Color {
        BLACK, WHITE;

        public static Color other(Color c) {
            return c == BLACK ? WHITE : BLACK;
        }
    }

    private Connection connection;
    private ChessFrame frame;
    private Board board;
    private Color nextColor;

    public GameController() {
        connection = new Connection();
        board = new Board();
    }

    public void start() {
        String host = frame.askInput("Please enter the address of the server.", "localhost");
        String portStr = frame.askInput("Please enter the port of the server.", "12345");
        int port;
        try {
            port = Integer.parseInt(portStr);
            connection.connect(host, port);
            createConnectionThread();
            newGame();
        } catch (NumberFormatException e) {
            frame.tell("Port must be a number.", true);
            start();
        } catch (IOException e) {
            frame.tell("Could not connect to server.", true);
            start();
        }
    }

    public void messageReceived(String msg) {
        String[] words = msg.split(" ");

        if (words[0].equals("say1")) {
            frame.appendChat("Player1:" + msg.substring(4));
        } else if (words[0].equals("say2")) {
            frame.appendChat("Player2:" + msg.substring(4));
        } else if (words[0].equals("color")) {
            frame.tell(words[1].equals("black") ?
                "You are playing with color black." :
                "You are playing with color white.");
        } else if (msg.equals("start")) {
            startGame();
        } else if (words[0].equals("error")) {
            if (words[1].equals("full")) {
                frame.tell("Sorry, the server is full.");
            } else if (words[1].equals("move")) {
                frame.tell("Invalid move.", true);
            } else if(words[1].equals("command")) {
                frame.tell("Internal error: the client sent a wrong command.", true);
            }
        } else if (msg.equals("resign")) {
            frame.tell("Resigned.");
            newGame();
        } else {

            // handling move commands

            if (words[0].equals("move")) {
                frame.appendLog(nextColor + " " + words[1] + "-" + words[2]);
                board.move(words[1], words[2]);
            } else if (words[0].equals("hit")) {
                frame.appendLog(nextColor + " " + words[1] + "x" + words[2]);
                board.hit(words[1], words[2]);
            } else if (words[0].equals("promote")) {
                frame.appendLog(nextColor + " " + words[1] + "-" + words[2] + " (Q)");
                board.promote(words[1], words[2]);
            } else if (words[0].equals("promote-hit")) {
                frame.appendLog(nextColor + " " + words[1] + "x" + words[2] + " (Q)");
                board.promoteHit(words[1], words[2]);
            } else if (words[0].equals("castle")) {
                boolean kingside = words[1].equals("kingside");
                frame.appendLog(nextColor + " " + (kingside ? "O-O" : "O-O-O"));
                board.castle(kingside, nextColor);
            }

            frame.update();

            if (hasGameEnded(words, words[0].equals("castle") ? 2 : 3)) {
                newGame();
            } else {
                nextColor = Color.other(nextColor);
                frame.changeCurrentPlayerLabel(nextColor.toString());
            }
        }

    }

    public void sendMove(String from, String to) {
        sendMessage("move " + from + " " + to);
    }

    public void sendResign() {
        sendMessage("resign");
    }

    public void sendChat(String text) {
        sendMessage("say " + text);
    }

    public Board getBoard() {
        return board;
    }

    public void setGui(ChessFrame chessFrame) {
        frame = chessFrame;
    }

    private void newGame() {
        frame.clearLog();
        frame.askReady();
        sendMessage("ready");
        frame.showWaiting();
    }

    private void startGame() {
        nextColor = Color.WHITE;
        board.reset();
        frame.changeCurrentPlayerLabel(nextColor.toString());
        frame.hideWaiting();
        frame.update();
    }

    private boolean hasGameEnded(String[] words, int idx) {
        if (words.length <= idx) {
            return false;
        }

        String w = words[idx];
        if (w.equals("check")) {
            frame.tell("Check");
        } else if (w.equals("checkmate")) {
            frame.tell("Checkmate");
            return true;
        } else if (w.equals("stalemate")) {
            frame.tell("Stalemate");
            return true;
        }

        return false;
    }

    private void sendMessage(String msg) {
        connection.send(msg);
    }

    private void createConnectionThread() {
        Runnable forwarder = new Runnable() {
            @Override
            public void run() {
                try {
                    connection.forwardInput(new Connection.Handler() {
                        @Override
                        public void message(String msg) {
                            messageReceived(msg);
                        }
                    });
                } catch (IOException e) {
                    System.err.println("Unexpected IO error.");
                    System.exit(1);
                }
            }
        };
        new Thread(forwarder).start();
    }

}
