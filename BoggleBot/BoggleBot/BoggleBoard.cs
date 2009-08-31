using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace BoggleBot
{
	class BoggleBoard
	{
		#region Declerations

		BoggleBoardData _data;
		BoggleWord _displayMove;
		int _curInput = 0;
		bool _curInputState = true;

		//drawing related data
		int _margin = 4;
		Pen _borderPen = new Pen(System.Drawing.Color.Black, 3);
		Pen _highlightBoxPen = new Pen(System.Drawing.Color.Green, 3);
		Pen _wordOutlinePen;

		Font _font;
		StringFormat _sf = new StringFormat();

		bool _recalcConstants = true;
		int _dx, _dy;
		Point _upperLeft, _lowerRight;

		#endregion

		#region Properties

		public bool CompleteBoard
		{
			get { return !_curInputState; }
		}

		#endregion

		#region Initialization

		public BoggleBoard(int size)
		{
			_data = new BoggleBoardData(size);
			//_data.GenerateRandom();
			Resized();
			_curInput = 0;
		}

		#endregion

		#region Public Methods

		public void ChangeBoardTo(int size)
		{
			if (size != _data.Size)
			{
				_data = new BoggleBoardData(size);
				//_data.GenerateRandom();
				Resized();
				_curInput = 0;
			}
		}

		public void KeyPress(char key)
		{
			if (key == 8) //handle backspace
			{
				if (!_curInputState)
					_curInputState = true;

				_curInput--;
				if (_curInput < 0)
					_curInput = 0;

				_data.SetChar(new BoardPos(_curInput, _data), (char)0);
				return;
			}

			//Handle a-z characters
			key = Char.ToLower(key);
			if (!_curInputState || key < 97 || key > 122)
				return;

			BoardPos input = new BoardPos(_curInput, _data);

			_data.SetChar(input, key);


			_curInput++;
			if (_curInput == _data.Size * _data.Size)
			{
				_curInputState = false;
			}
		}

		public void GenerateRandom()
		{
			_data.GenerateRandom();
			_curInput = _data.Size * _data.Size;
			_curInputState = false;
		}

		public void SetDisplayMove(BoggleWord word)
		{
			_displayMove = word;
		}

		#endregion

		#region Draw

		public void Paint(Panel panel, PaintEventArgs e)
		{
			if (_recalcConstants)
				RecalcDrawingConstants(panel.DisplayRectangle);

			//Draw the board itself
			Point pos = _upperLeft;
			for (int line = 0; line <= _data.Size; line++)
			{
				e.Graphics.DrawLine(_borderPen, new Point(_upperLeft.X, pos.Y), new Point(_lowerRight.X, pos.Y));
				e.Graphics.DrawLine(_borderPen, new Point(pos.X, _upperLeft.Y), new Point(pos.X, _lowerRight.Y));

				pos.Y += _dy;
				pos.X += _dx;
			}

			//Draw the text onto the board
			for (int x = 0; x < _data.Size; x++)
			{
				for (int y = 0; y < _data.Size; y++)
				{
					BoardPos bpos = new BoardPos(x, y);

					Rectangle location = GetScreenPos(bpos);
					location.Height = _font.Height;

					char output = _data.GetChar(bpos);
					if (output == 0)
						continue;

					string test = new String(output, 1).ToUpper();
					e.Graphics.DrawString(test, _font, Brushes.Black, location, _sf);
				}
			}

			//Draw highlight
			if (_curInputState)
				e.Graphics.DrawRectangle(_highlightBoxPen, GetScreenPos(new BoardPos(_curInput, _data)));

			//Draw _displayMove
			if (_displayMove != null)
			{
				Rectangle sr1 = GetScreenPos(_displayMove._moves[0]);
				Point sp1 = new Point(sr1.X + _dx / 2, sr1.Y + _dy / 2);
				Point sp2 = new Point(sp1.X, sp1.Y - _dy / 3);

				e.Graphics.DrawLine(_wordOutlinePen, sp1, sp2);


				for (int i = 1; i < _displayMove._moves.Count; i++)
				{
					Rectangle r1 = GetScreenPos(_displayMove._moves[i - 1]);
					Point p1 = new Point(r1.X + _dx / 2, r1.Y + _dy / 2);

					Rectangle r2 = GetScreenPos(_displayMove._moves[i]);
					Point p2 = new Point(r2.X + _dx / 2, r2.Y + _dy / 2);

					e.Graphics.DrawLine(_wordOutlinePen, p1, p2);
				}
			}
		}

		/// <summary>
		/// Called to force it to recalculation drawing constants next refresh
		/// </summary>
		public void Resized()
		{
			_recalcConstants = true;
		}

		private void RecalcDrawingConstants(Rectangle rect)
		{
			int theight = rect.Height - (2 * _margin);
			int twidth = rect.Width - (2 * _margin);

			_dx = twidth / _data.Size;
			_dy = theight / _data.Size;

			_upperLeft = new Point(_margin, _margin);
			_lowerRight = new Point(_upperLeft.X + _data.Size * _dx, _upperLeft.Y + _data.Size * _dy);


			_font = new Font("Arial", (int)((float)(_dy - 8) * 0.75f) + 4);
			_sf.Alignment = StringAlignment.Center;
			_sf.LineAlignment = StringAlignment.Center;


			_wordOutlinePen = new Pen(Color.FromArgb(50, Color.Red), _dy / 3);
			_wordOutlinePen.StartCap = LineCap.Round;
			_wordOutlinePen.EndCap = LineCap.Round;


			_recalcConstants = false;
		}

		//need function to convert board point to screen space
		//both upper left and center

		#endregion

		#region Positioning

		public Rectangle GetScreenPos(BoardPos pos)
		{
			return new Rectangle(_upperLeft.X + pos._x * _dx, _upperLeft.Y + pos._y * _dy, _dx, _dy);
		}

		#endregion
	}

	/// <summary>
	/// Data representing a board
	/// </summary>
	public class BoggleBoardData
	{
		#region Declerations

		const int USED_POS_FLAG = 0x80; //b10000000, store if this board position is used in a current search in upper most bit
		const int CHAR_DATA_MASK = 0x7f; //0b01111111

		byte[] _boardData;
		int _size;

		#endregion

		#region Properties

		public int Size
		{
			get { return _size; }
		}

		#endregion

		#region Initialization

		public BoggleBoardData(int size)
		{
			_boardData = new byte[size * size];
			_size = size;
		}

		#endregion

		#region Public Methods

		public char GetChar(BoardPos pos)
		{
			return (char)(_boardData[pos._x + pos._y * _size] & CHAR_DATA_MASK);
		}

		public void SetChar(BoardPos pos, char value)
		{
			_boardData[pos._x + pos._y * _size] = (byte)value;
		}

		public void GenerateRandom()
		{
			Random r = new Random();

			for (int i = 0; i < _size * _size; i++)
			{
				byte t = (byte)r.Next((int)'a', (int)'z');
				_boardData[i] = t;
			}
		}

		public List<BoardPos> PossibleMovesFrom(BoardPos curPos)
		{
			List<BoardPos> moves = new List<BoardPos>();

			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx == 0 && dy == 0)
						continue;

					BoardPos testPos = new BoardPos(curPos._x + dx, curPos._y + dy);

					if (!UsedPos(testPos))
						moves.Add(testPos);
				}
			}

			return moves;
		}

		public void MarkUsed(BoardPos pos)
		{
			_boardData[pos._y * _size + pos._x] |= USED_POS_FLAG;
		}
		public void MarkNotUsed(BoardPos pos)
		{
			_boardData[pos._y * _size + pos._x] ^= USED_POS_FLAG;
		}

		#endregion

		#region Private Methods

		private bool UsedPos(BoardPos pos)
		{
			return (BoardData(pos) & USED_POS_FLAG) != 0;
		}

		private byte BoardData(BoardPos pos)
		{
			return _boardData[pos._y * _size + pos._x];
		}

		#endregion

	}

	/// <summary>
	/// A position on the board, 0,0 = upper left
	/// </summary>
	public struct BoardPos
	{
		public int _x, _y;

		public BoardPos(int x, int y)
		{
			_x = x;
			_y = y;
		}
		public BoardPos(int p, BoggleBoardData board)
		{
			_x = p % board.Size;
			_y = p / board.Size;
		}
	}
}
