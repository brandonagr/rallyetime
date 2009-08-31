using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BoggleBot
{
    class BoggleBot
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

        Board _board = new Board(4);

        #endregion

        #region Initialization

        public BoggleBot()
        {
            _fdt = new FastDictionaryTree();

        }

        #endregion
    }

    /// <summary>
    /// The boggle board data
    /// </summary>
    struct Board
    {
        #region Declerations

        const int USED_POS_FLAG = 0x80; //b10000000, store if this board position is used in a current search in upper most bit

        byte[] _boardData;
        int _size;
        
        #endregion

        #region Properties
        

        #endregion

        #region Initialization

        public Board(int size)
        {

            _boardData = new byte[size * size];
            _size = size;
        }

        #endregion


        #region Public Methods

        public void GenerateRandom()
        {
            Random r = new Random();

            for(int i=0; i<_size*_size; i++)
                _boardData[i] = (byte)r.Next((int)'a', (int)'z');
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
    /// A position on the board
    /// </summary>
    public struct BoardPos
    {
        public int _x, _y;

        public BoardPos(int x, int y)
        {
            _x = x;
            _y = y;
        }
    }
}
