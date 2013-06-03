package src;

import java.util.HashMap;
import java.util.Map;

import src.GameController.Color;
import src.GameController.Piece;

/**
 * A class which encapsulates the position of both the white and black pieces on
 * the chessboard separately.
 * 
 * @author szabolcs
 */

public class Board {

	private Map<String, Piece> blackPieces, whitePieces;

	public Board() {
		blackPieces = new HashMap<String, Piece>(16);
		whitePieces = new HashMap<String, Piece>(16);
	}

	public void reset() {
		blackPieces.clear();
		whitePieces.clear();
		initBoard();
	}

	public void move(String from, String to) {
		Piece p = blackPieces.get(from);
		if (p == null) {
			p = whitePieces.get(from);
			whitePieces.put(to, p);
			whitePieces.remove(from);
		} else {
			blackPieces.put(to, p);
			blackPieces.remove(from);
		}
	}

	public void hit(String from, String to) {
		Piece p = blackPieces.get(to);
		if (p == null) {
			whitePieces.remove(to);
		} else {
			blackPieces.remove(to);
		}
		move(from, to);
	}

	public void promote(String from, String to) {
		Piece p = blackPieces.get(from);
		if (p == null) {
			p = whitePieces.get(from);
			whitePieces.put(to, Piece.QUEEN);
			whitePieces.remove(from);
		} else {
			blackPieces.put(to, Piece.QUEEN);
			blackPieces.remove(from);
		}
	}

	public void promoteHit(String from, String to) {
		Piece p = blackPieces.get(to);
		if (p == null) {
			whitePieces.remove(to);
		} else {
			blackPieces.remove(to);
		}
		promote(from, to);
	}

	public void castle(boolean kingside, Color color) {
		String from_king = "", from_rook = "", to_king = "", to_rook = "";
		if (kingside) {
			switch (color) {
			case WHITE:
				from_king = "e1";
				from_rook = "h1";
				to_king = "g1";
				to_rook = "f1";
				break;
			case BLACK:
				from_king = "e8";
				from_rook = "h8";
				to_king = "g8";
				to_rook = "f8";
				break;
			}
		} else {
			switch (color) {
			case WHITE:
				from_king = "e1";
				from_rook = "a1";
				to_king = "c1";
				to_rook = "d1";
				break;
			case BLACK:
				from_king = "e8";
				from_rook = "a8";
				to_king = "c8";
				to_rook = "d8";
				break;
			}
		}
		move(from_king, to_king);
		move(from_rook, to_rook);
	}

	public Map<String, Piece> getBlackPieces() {
		return blackPieces;
	}

	public void setBlackPieces(Map<String, Piece> blackPieces) {
		this.blackPieces = blackPieces;
	}

	public Map<String, Piece> getWhitePieces() {
		return whitePieces;
	}

	public void setWhitePieces(Map<String, Piece> whitePieces) {
		this.whitePieces = whitePieces;
	}

	private String concat(char c, int i) {
		return String.valueOf(c).concat(String.valueOf(i));
	}

	private void initBoard() {
		final Piece[] firstRow = { Piece.ROOK, Piece.KNIGHT, Piece.BISHOP,
				Piece.QUEEN, Piece.KING, Piece.BISHOP, Piece.KNIGHT, Piece.ROOK };

		final char[] labels = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

		for (int i = 0; i < 8; i++) {
			blackPieces.put(concat(labels[i], 8), firstRow[i]);
			whitePieces.put(concat(labels[i], 1), firstRow[i]);

			blackPieces.put(concat(labels[i], 7), Piece.PAWN);
			whitePieces.put(concat(labels[i], 2), Piece.PAWN);
		}
	}
}
