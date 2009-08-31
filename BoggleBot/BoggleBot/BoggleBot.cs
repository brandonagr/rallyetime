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

		// vars used while finding solution instead of passing a bunch of stuff to each function call
		List<BoggleSolution> _curSolutionList;
		Stack<BoardPos> _curMoveHistory;
		BoggleBoardData _curBoard;
		string _curWord;


		#endregion

		#region Initialization

		public BogglePlayer()
		{
			_fdt = new FastDictionaryTree();
			_fdt.AddWordsFromFile("words.txt");
		}

		#endregion

		#region Solution

		public List<BoggleSolution> FindSolution(BoggleBoardData data)
		{
			_curSolutionList = new List<BoggleSolution>();
			_curMoveHistory = new Stack<BoardPos>();
			_curBoard = data;

			for (int x = 0; x < data.Size; x++)
			{
				for (int y = 0; y < data.Size; y++)
				{
					BoardPos pos = new BoardPos(x, y);					

					FDTNode curNode = _fdt.Root;
					_curWord = "";
					char curLetter = _curBoard.GetChar(pos);

					if (curNode.HasChild(curLetter))
					{
						_curMoveHistory.Push(pos);
						_curWord = _curWord + curLetter;
						FindSolutionRecurse(curNode.Child(curLetter));
						_curWord = _curWord.Substring(0, _curWord.Length-1);
						_curMoveHistory.Pop();
					}					
				}
			}

			return _curSolutionList;
		}

		private void FindSolutionRecurse(FDTNode curNode)
		{
			//currently at BoardPos _curMoveHistory.Peek

			_curBoard.MarkUsed(_curMoveHistory.Peek());

			if (curNode.IsWord && _curWord.Length >= 3)
			{
				//make sure _curWord isn't already in the list
				bool newWord = true;
				foreach (BoggleSolution sol in _curSolutionList)
				{
					if (sol._word == _curWord)
					{
						newWord = false;
						break;
					}
				}
				if (newWord)
				{
					BoggleSolution sol = new BoggleSolution();

					sol._word = _curWord;

					BoardPos[] history = _curMoveHistory.ToArray();
					for(int i = history.Length-1; i>=0; i--)					
					{
						sol._moves.Add(history[i]);
					}

					_curSolutionList.Add(sol);
				}
			}

			List<BoardPos> moves = _curBoard.PossibleMovesFrom(_curMoveHistory.Peek());

			foreach (BoardPos move in moves)
			{
				char curLetter = _curBoard.GetChar(move);

				if (curNode.HasChild(curLetter))
				{
					_curMoveHistory.Push(move);
					_curWord = _curWord + curLetter;
					FindSolutionRecurse(curNode.Child(curLetter));
					_curWord = _curWord.Substring(0, _curWord.Length - 1);
					_curMoveHistory.Pop();
				}
			}

			_curBoard.MarkNotUsed(_curMoveHistory.Peek());
		}

		#endregion
	}

	public class BoggleSolution
	{
		public string _word = string.Empty;
		public List<BoardPos> _moves = new List<BoardPos>();

		public override string ToString()
		{
			return _word;
		}
	}
}
