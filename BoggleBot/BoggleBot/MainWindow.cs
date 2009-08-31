using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BoggleBot
{
    public partial class BoggleBotWindow : Form
    {
        int _boardSize = 4;

        public BoggleBotWindow()
        {
            InitializeComponent();

            gridSize_ComboBox.SelectedIndex = 0;
            _boardSize = gridSize_ComboBox.SelectedIndex + 4;

            DrawBoard();
        }

        public void DrawBoard()
        {
            System.Drawing.Graphics formGraphics = this.CreateGraphics();

            SolidBrush solidBrush = new SolidBrush(Color.DarkCyan);
            formGraphics.FillEllipse(solidBrush, 10, 10, 100, 30);

            formGraphics.Dispose();

        }

        /// <summary>
        /// Reset the BoggleBot
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gridSize_ComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _boardSize = gridSize_ComboBox.SelectedIndex + 4;
            panel1.Refresh();
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            Panel panel = (Panel)sender;

            Pen border = new Pen(System.Drawing.Color.Black, 3);

            Point upper_left = new Point(5, 5);
            Point lower_right = new Point(panel.DisplayRectangle.Width - 5, panel.DisplayRectangle.Height - 5);

            int height = lower_right.Y - upper_left.Y;
            int width = lower_right.X - upper_left.X;

            int dy = height / _boardSize;
            int dx = width / _boardSize;
            int y = upper_left.Y;
            int x = upper_left.X;

            Point actual_lower_right = new Point(upper_left.X + _boardSize * dx, upper_left.Y + _boardSize * dy);

            for (int line = 0; line <= _boardSize; line++)
            {
                e.Graphics.DrawLine(border, new Point(upper_left.X, y), new Point(actual_lower_right.X, y));
                e.Graphics.DrawLine(border, new Point(x, upper_left.Y), new Point(x, actual_lower_right.Y));

                y += dy;
                x += dx;
            }
            
        }
    }
}
