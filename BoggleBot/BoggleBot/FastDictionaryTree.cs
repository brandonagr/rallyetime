using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Runtime.Serialization;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text.RegularExpressions;

namespace BoggleBot
{
	/// <summary>
	/// A tree structure to store a word list
	/// </summary>
	public class FastDictionaryTree
	{
		#region Declerations

		FDTNode _root = new FDTNode();

		#endregion

		#region Initialization

		public FastDictionaryTree()
		{ }

		#endregion

		#region Public Methods

		public void PrintTree()
		{
			PrintTree(Console.Out);
		}

		public void WriteToFile(string filename)
		{
			using (TextWriter wr = new StreamWriter(filename))
			{
				PrintTree(wr);
			}
		}

		public void AddWord(string word)
		{
			if (word.Length == 0)
				throw new ArgumentException();

			_root.AddToChildren(word);
		}

		public int AddWordsFromFile(string filename)
		{
			int added = 0;

			using (StreamReader sr = File.OpenText(filename))
			{
				Regex re = new Regex("^[a-zA-Z]+$");

				string line = sr.ReadLine().Trim();
				while (line != null)
				{
					line = line.Trim();

					if (re.IsMatch(line))
					{
						AddWord(line.ToLower());
						added++;
					}

					line = sr.ReadLine();
				}
			}

			return added;
		}

		#endregion

		#region Private Methods

		private void PrintTree(TextWriter wr)
		{
			Stack<char> curWord = new Stack<char>();

			PrintTreeRecurse(_root, curWord, wr);
		}

		private void PrintTreeRecurse(FDTNode node, Stack<char> curWord, TextWriter wr)
		{
			if (node.IsWord)
				wr.WriteLine(Util.ReverseString(new string(curWord.ToArray())));

			if (node.HasChildren)
			{
				char letter = 'a';
				foreach (FDTNode child in node.Children)
				{
					curWord.Push(letter);
					if (child != null)
						PrintTreeRecurse(child, curWord, wr);
					curWord.Pop();
					letter++;
				}
			}
		}

		#endregion
	}

	/// <summary>
	/// Node in Fast Dictionary Tree, what letter this node represents is only stored by it's parent, ie what position in the FDTNode[] points to this node
	/// </summary>
	public class FDTNode
	{
		#region Declerations

		[Flags]
		enum NodeFlags : byte
		{
			EMPTY = 0x0,
			IS_WORD = 0x1,
			HAS_CHILDREN = 0x2,
			NUMB_FLAGS = 0x4
		};

		NodeFlags _flags = NodeFlags.EMPTY;
		FDTNode[] _children = null;

		#endregion

		#region Properties

		/// <summary>
		/// Does this node represent a complete word
		/// </summary>
		public bool IsWord
		{
			get { return (_flags & NodeFlags.IS_WORD) != 0; }
			set
			{
				if (value)
					_flags |= NodeFlags.IS_WORD;
				else
					_flags ^= NodeFlags.IS_WORD;
			}
		}

		/// <summary>
		/// Does this node have child nodes (is it not a leaf)
		/// </summary>
		public bool HasChildren
		{
			get { return (_flags & NodeFlags.HAS_CHILDREN) != 0; }
			set
			{
				if (value && !HasChildren)
				{
					_flags |= NodeFlags.HAS_CHILDREN;
					_children = new FDTNode[26];
				}
			}
		}

		public FDTNode[] Children
		{
			get { return _children; }
		}

		#endregion

		#region Initialization

		/// <summary>
		/// Nothing to initialize
		/// </summary>
		public FDTNode()
		{ }

		#endregion

		#region Public Methods

		public void AddToChildren(string word)
		{
			if (word.Length == 0)
			{
				IsWord = true;
				return;
			}

			char letter = word[0];
			int index = Util.Index(letter);

			//if it didn't have children before this will allocate the FDTNode[] array
			HasChildren = true;

			if (_children[index] == null)
				_children[index] = new FDTNode();

			_children[index].AddToChildren(word.Substring(1));
		}

		#endregion
	}


	/// <summary>
	/// Utility functions
	/// </summary>
	public static class Util
	{
		/// <summary>
		/// Convert char letter to an index for an array with 26 elements
		/// </summary>
		/// <param name="letter"></param>
		/// <returns></returns>
		public static int Index(char letter)
		{
			if (letter < 97 || letter > 122)
				throw new ArgumentException("letter out of range");

			return (int)(letter - 97);
		}

		/// <summary>
		/// Convert int array index to the letter it represents
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		public static char Letter(int index)
		{
			if (index < 0 || index > 26)
				throw new ArgumentException("index out of range");

			return (char)(index + 97);
		}

		/// <summary>
		/// Receives string and returns the string with its letters reversed.
		/// </summary>
		public static string ReverseString(string s)
		{
			char[] arr = s.ToCharArray();
			Array.Reverse(arr);
			return new string(arr);
		}
	}
}
