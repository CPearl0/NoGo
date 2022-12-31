#include "pch.h"
#include "Chess.h"

inline Piece operator-(Piece p)
{
    switch (p)
    {
    case Piece::Black:
        return Piece::White;
    case Piece::White:
        return Piece::Black;
    }
    return Piece::None;
}

bool CChess::CanPlacePiece(int x, int y) const
{
    if (Result != ChessResult::None) return false;
    if (x < 0 || x >= Length || y < 0 || y >= Length) return false;
    if (Chess[x][y] != Piece::None) return false;
    return true;
}

bool CChess::PlacePiece(int x, int y)
{
    ASSERT(CanPlacePiece(x, y));
    if (Next == Piece::None) return false;
    Result = LazyJudge(x, y, Next);
    Chess[x][y] = Next;
    Record.push_back((x << 4) + y);
    ++Step;
    if (Step == Length * Length)
        Result = ChessResult::Draw;
    if (Result != ChessResult::None)
    {
        Next = Piece::None;
        return true;
    }
    Next = -Next;
    return false;
}

bool CChess::GoNext()
{
    if (Step == Record.size()) return true;
    byte b = Record[Step];
    Chess[b >> 4][b & 0xf] = (Step % 2) ? Piece::White : Piece::Black;
    if (++Step == Record.size())
    {
        if (Result == ChessResult::None)
            Next = (Step % 2) ? Piece::White : Piece::Black;
        return true; 
    }
    Next = Piece::None;
    return false;
}

bool CChess::GoPrev()
{
    if (Step == 0) return true;
    byte b = Record[Step - 1];
    Chess[b >> 4][b & 0xf] = Piece::None;
    Next = Piece::None;
    return --Step == 0;
}

void CChess::GoStart()
{
    int i, j;
    for (i = 0; i < Length; i++)
        for (j = 0; j < Length; j++)
            Chess[i][j] = Piece::None;
    Next = Piece::None;
    Step = 0;
}

void CChess::GoEnd()
{
    bool black = true;
    for (byte b : Record)
    {
        Chess[b >> 4][b & 0xf] = black ? Piece::Black : Piece::White;
        black = !black;
    }
    if (Result == ChessResult::None)
        Next = black ? Piece::Black : Piece::White;
    Step = Record.size();
}

void CChess::Reset()
{
    if (Step >= Record.size()) return;
    Record.resize(Step);
    Next = Record.size() % 2 ? Piece::White : Piece::Black;
    Result = ChessResult::None;
}
