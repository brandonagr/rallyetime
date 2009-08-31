using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BoggleBot
{
	public class BogglePlayer
	{
		#region Declerations

		enum State
		{
			WAITING_FOR_BOARD,
			SOLVING,
			DONE
		}

		State _state = State.WAITING_FOR_BOARD;
		FastDictionaryTree _fdt;


		#endregion

		#region Initialization

		public BogglePlayer()
		{
			_fdt = new FastDictionaryTree();

		}

		#endregion
	}

	public class BoggleWord
	{
		public string _word = string.Empty;
		public List<BoardPos> _moves = new List<BoardPos>();
	}
}
