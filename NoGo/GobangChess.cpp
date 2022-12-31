#include "pch.h"
#include "GobangChess.h"
#include <random>

ChessResult CGobangChess::LazyJudge(int x, int y, Piece p) const
{
    byte s = 1, a = 1;
    while (x - s >= 0 && Chess[x - s][y] == p) ++s;
    while (x + a < Length && Chess[x + a][y] == p) ++a;
    if (s + a > 5) return static_cast<ChessResult>(p);
    s = a = 1;
    while (y - s >= 0 && Chess[x][y - s] == p) ++s;
    while (y + a < Length && Chess[x][y + a] == p) ++a;
    if (s + a > 5) return static_cast<ChessResult>(p);
    s = a = 1;
    while (x - s >= 0 && y - s >= 0 && Chess[x - s][y - s] == p) ++s;
    while (x + a < Length && y + a < Length && Chess[x + a][y + a] == p) ++a;
    if (s + a > 5) return static_cast<ChessResult>(p);
    s = a = 1;
    while (x + s < Length && y - s >= 0 && Chess[x + s][y - s] == p) ++s;
    while (x - a >= 0 && y + a < Length && Chess[x - a][y + a] == p) ++a;
    if (s + a > 5) return static_cast<ChessResult>(p);
    if (Step == Length * Length - 1) return ChessResult::Draw;
    return ChessResult::None;
}

byte CGobangChess::AIResponse(byte AIType) const
{
    switch (AIType)
    {
    case 0: return SimpleRandomAI();
    default: ASSERT(FALSE);
    }
}

byte CGobangChess::SimpleRandomAI() const
{
    int i, j;
    std::vector<byte> possible;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (CanPlacePiece(i, j))
                if (LazyJudge(i, j, Next) == static_cast<ChessResult>(Next))
                    return (i << 4) + j;
                else if (LazyJudge(i, j, -Next) == static_cast<ChessResult>(-Next))
                    return (i << 4) + j;
                else
                    possible.push_back((i << 4) + j);
    if (possible.empty())
        for (i = 0; i < Length; ++i)
            for (j = 0; j < Length; ++j)
                if (CanPlacePiece(i, j))
                    return (i << 4) + j;
    std::random_device rd;
    return possible[rd() % possible.size()];
}