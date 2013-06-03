package src;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class ChessFrame extends JFrame {

	public static final int OFFSET = 50;
	private BoardPanel boardPanel;
	private PiecePanel piecePanel;
	private GameController game;
	private JTextArea logTextArea;
	private JTextArea chatTextArea;
	private JLabel currentPlayerLabel;

	private String prevSquare;
	private CardLayout cardLayout;

	public ChessFrame(GameController gc) {
		this.game = gc;
		gc.setGui(this);

		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setResizable(false);
		setTitle("Chess");
		cardLayout = new CardLayout();
		setLayout(cardLayout);

		JPanel waitingPanel = new JPanel();
		waitingPanel.add(new JLabel("Waiting for the other player..."));

		JPanel gamePanel = new JPanel(new BorderLayout());

		JLayeredPane layeredPane = new JLayeredPane();
		layeredPane.setPreferredSize(new Dimension(8 * OFFSET, 8 * OFFSET));
		layeredPane.setBorder(BorderFactory.createLineBorder(Color.black));
		gamePanel.add(layeredPane, BorderLayout.CENTER);

		logTextArea = new JTextArea(7, 10);
		logTextArea.setBorder(BorderFactory.createTitledBorder("Log"));
		logTextArea.setEditable(false);
		logTextArea.setSize(new Dimension(layeredPane.getWidth(), 100));

		GridBagLayout gridBagLayout = new GridBagLayout();
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.insets = new Insets(6, 3, 6, 3);
		gbc.gridwidth = 1;
		gbc.gridx = 0;
		gbc.gridy = GridBagConstraints.RELATIVE;

		JPanel controlPanel = new JPanel(gridBagLayout);
		controlPanel.setBorder(BorderFactory.createTitledBorder("Control"));

		controlPanel.add(new JButton(new AbstractAction("Resign") {
			@Override
			public void actionPerformed(ActionEvent e) {
				game.sendResign();
			}
		}), gbc);

		gbc.fill = GridBagConstraints.HORIZONTAL;

		currentPlayerLabel = new JLabel("Current player: white");
		controlPanel.add(currentPlayerLabel, gbc);

		chatTextArea = new JTextArea(10, 20);
		chatTextArea.setBorder(BorderFactory.createTitledBorder("Chat"));
		chatTextArea.setEditable(false);
		gbc.fill = GridBagConstraints.BOTH;
		gbc.weighty = 1;
		controlPanel.add(new JScrollPane(chatTextArea), gbc);

		JTextField chatField = new JTextField();
		chatField.addActionListener(new AbstractAction() {

			@Override
			public void actionPerformed(ActionEvent e) {
				JTextField source = (JTextField) e.getSource();
				String text = source.getText();
				game.sendChat(text);
				source.setText("");
			}
		});

		gbc.weighty = 0;
		controlPanel.add(chatField, gbc);

		gamePanel.add(controlPanel, BorderLayout.EAST);

		gamePanel.add(new JScrollPane(logTextArea), BorderLayout.SOUTH);

		boardPanel = new BoardPanel(OFFSET);

		piecePanel = new PiecePanel(game.getBoard(), OFFSET);
		piecePanel.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				char[] str = new char[2];
				str[0] = (char) ((int) 'a' + (e.getX() / OFFSET));
				str[1] = (char) ((int) '8' - (e.getY() / OFFSET));

				String square = String.copyValueOf(str);
				if (prevSquare == null) {
					prevSquare = square;
				} else {
					game.sendMove(prevSquare, square);
					prevSquare = null;
				}
			}
		});

		layeredPane.add(boardPanel, new Integer(0));
		layeredPane.add(piecePanel, new Integer(1));

		add(gamePanel, "gamePanel");
		add(waitingPanel, "waitingPanel");

		pack();
	}

	public void update() {
		piecePanel.repaint();
	}

	public void appendLog(String newLine) {
		logTextArea.append(newLine + "\n");
		logTextArea.setCaretPosition(logTextArea.getDocument().getLength());
	}

	public void appendChat(String msg) {
		chatTextArea.append(msg + "\n");
		chatTextArea.setCaretPosition(chatTextArea.getDocument().getLength());
	}

	public void changeCurrentPlayerLabel(String msg) {
		currentPlayerLabel.setText("Current player: " + msg);
	}

	public void clearLog() {
		logTextArea.setText("");
	}

	public void askReady() {
		JOptionPane
				.showMessageDialog(this,
						"Click OK if you are ready. The game begins when both players are ready.");
	}

	public String askInput(String prompt, String def) {
		return JOptionPane.showInputDialog(this, prompt, def);
	}

	public void tell(String string) {
		tell(string, false);
	}

	public void tell(String string, boolean error) {
		JOptionPane.showMessageDialog(this, string, "Game",
				(error ? JOptionPane.ERROR_MESSAGE
						: JOptionPane.INFORMATION_MESSAGE), null);
	}

	public void showWaiting() {
		cardLayout.show(getContentPane(), "waitingPanel");
	}

	public void hideWaiting() {
		cardLayout.show(getContentPane(), "gamePanel");
	}
}
