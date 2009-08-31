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
		#region Declerations

		BogglePlayer _bot;
		BoggleBoard _board;

		#endregion

		public BoggleBotWindow()
		{
			InitializeComponent();
			this.KeyPreview = true;

			_bot = new BogglePlayer();
			_board = new BoggleBoard(gridSize_ComboBox.SelectedIndex + 4);

			gridSize_ComboBox.SelectedIndex = 0;



			/*
			BoggleSolution word = new BoggleSolution();
			word._moves.Add(new BoardPos(0, 0));
			word._moves.Add(new BoardPos(1, 1));
			word._moves.Add(new BoardPos(2, 1));
			word._moves.Add(new BoardPos(3, 1));
			word._moves.Add(new BoardPos(2, 2));

			_board.SetDisplayMove(word);*/
		}

		private void gridSize_ComboBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			_board.ChangeBoardTo(gridSize_ComboBox.SelectedIndex + 3);

			_board.SetDisplayMove(null);
			listBox1.DataSource = null;

			panel1.Refresh();
			panel1.Focus();
		}

		private void panel1_Paint(object sender, PaintEventArgs e)
		{
			_board.Paint((Panel)sender, e);
		}

		private void BoggleBotWindow_Resize(object sender, EventArgs e)
		{
			_board.Resized();
			panel1.Invalidate();
		}

		private void BoggleBotWindow_KeyPress(object sender, KeyPressEventArgs e)
		{
			_board.KeyPress(e.KeyChar);
			panel1.Invalidate();

			if (_board.CompleteBoard)
			{
				listBox1.DataSource = _bot.FindSolution(_board.BoardData);
			}
		}

		private void randomGrid_Button_Click(object sender, EventArgs e)
		{
			_board.GenerateRandom();
			panel1.Invalidate();

			if (_board.CompleteBoard)
			{
				listBox1.DataSource = _bot.FindSolution(_board.BoardData);
			}
		}

		private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			_board.SetDisplayMove((BoggleSolution)(((ListBox)sender).SelectedValue));
			panel1.Invalidate();
		}
	}

	public class DoubleBufferPanel : Panel
	{
		public DoubleBufferPanel()
		{
			this.SetStyle(
				ControlStyles.UserPaint |
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.OptimizedDoubleBuffer, true);
			UpdateStyles();
		}
	}
}
