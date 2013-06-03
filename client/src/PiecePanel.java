package src;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.Map;

import javax.imageio.ImageIO;
import javax.swing.JPanel;

import src.GameController.Piece;

public class PiecePanel extends JPanel {

	private Board board;
	private int offset;
	private BufferedImage whiteRook, whiteKnight, whiteBishop, whiteQueen,
			whiteKing, whitePawn, blackRook, blackKnight, blackBishop,
			blackQueen, blackKing, blackPawn;

	public PiecePanel(Board board, int offset) {
		this.board = board;
		this.offset = offset;
		setBounds(0, 0, 8 * offset, 8 * offset);
		setOpaque(false);
		try {
			loadResources();
		} catch (IOException e) {
			System.err.println(e.getMessage());
		}
	}

	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		Graphics2D g2 = (Graphics2D) g;

		// draw black and white pieces respectively
		for (Map.Entry<String, Piece> entry : board.getBlackPieces().entrySet()) {
			Point pos = parseKey(entry.getKey());

			switch (entry.getValue()) {
			case ROOK:
				g2.drawImage(blackRook, pos.x, pos.y, null);
				break;
			case KNIGHT:
				g2.drawImage(blackKnight, pos.x, pos.y, null);
				break;
			case BISHOP:
				g2.drawImage(blackBishop, pos.x, pos.y, null);
				break;
			case KING:
				g2.drawImage(blackKing, pos.x, pos.y, null);
				break;
			case QUEEN:
				g2.drawImage(blackQueen, pos.x, pos.y, null);
				break;
			case PAWN:
				g2.drawImage(blackPawn, pos.x, pos.y, null);
				break;

			}
		}

		for (Map.Entry<String, Piece> entry : board.getWhitePieces().entrySet()) {
			Point pos = parseKey(entry.getKey());

			switch (entry.getValue()) {
			case ROOK:
				g2.drawImage(whiteRook, pos.x, pos.y, null);
				break;
			case KNIGHT:
				g2.drawImage(whiteKnight, pos.x, pos.y, null);
				break;
			case BISHOP:
				g2.drawImage(whiteBishop, pos.x, pos.y, null);
				break;
			case KING:
				g2.drawImage(whiteKing, pos.x, pos.y, null);
				break;
			case QUEEN:
				g2.drawImage(whiteQueen, pos.x, pos.y, null);
				break;
			case PAWN:
				g2.drawImage(whitePawn, pos.x, pos.y, null);
				break;

			}
		}
	}

	private Point parseKey(String str) {
		String files = "abcdefgh";
		String ranks = "87654321";
		int dx = files.indexOf(str.charAt(0)) * offset;
		int dy = ranks.indexOf(str.charAt(1)) * offset;

		return new Point(dx, dy);
	}

	private void loadResources() throws IOException {
		final String sep = System.getProperty("file.separator");
		final String path = "resource" + sep;

		whiteRook = ImageIO.read(new File(path + "white_rook.png"));
		whiteKnight = ImageIO.read(new File(path + "white_knight.png"));
		whiteBishop = ImageIO.read(new File(path + "white_bishop.png"));
		whiteKing = ImageIO.read(new File(path + "white_king.png"));
		whiteQueen = ImageIO.read(new File(path + "white_queen.png"));
		whitePawn = ImageIO.read(new File(path + "white_pawn.png"));

		blackRook = ImageIO.read(new File(path + "black_rook.png"));
		blackKnight = ImageIO.read(new File(path + "black_knight.png"));
		blackBishop = ImageIO.read(new File(path + "black_bishop.png"));
		blackKing = ImageIO.read(new File(path + "black_king.png"));
		blackQueen = ImageIO.read(new File(path + "black_queen.png"));
		blackPawn = ImageIO.read(new File(path + "black_pawn.png"));
	}
}
