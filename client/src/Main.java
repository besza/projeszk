package src;

import javax.swing.*;

public class Main {

	public static void main(String[] args) {
        try {
            UIManager.setLookAndFeel(
                UIManager.getSystemLookAndFeelClassName());
        } 
        catch (Exception e) { }
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				GameController game = new GameController();
				new ChessFrame(game).setVisible(true);
				game.start();
			}
		});
	}
}
