package src;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;

import javax.swing.JPanel;

public class BoardPanel extends JPanel {

    private int offset;

    public BoardPanel(int offset) {
        this.offset = offset;
        setBounds(0, 0, 8 * offset, 8 * offset);
        setOpaque(true);
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;

        // draw the tiles
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Rectangle2D rect = new Rectangle2D.Double(i * offset, j
                        * offset, offset, offset);

                boolean white = i % 2 == 0;
                if (j % 2 != 0)
                    white = !white;

                g2.setColor(white ? Color.white : Color.cyan);
                g2.fill(rect);
                g2.draw(rect);
            }
        }

    }

}
