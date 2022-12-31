#include "pch.h"
#include "NoGoChess.h"
#include <queue>
#include <unordered_set>
#include <random>

bool CNoGoChess::CanPlacePiece(int x, int y) const
{
    if (!CChess::CanPlacePiece(x, y)) return false;
    if (Step == 0 && 2 * x + 1 == Length && 2 * y + 1 == Length) return false;
    return true;
}

ChessResult CNoGoChess::LazyJudge(int x, int y, Piece p) const
{
    if (Chess[x][y] != Piece::None) return ChessResult::None;
    const_cast<CNoGoChess*>(this)->Chess[x][y] = p;
    p = -p;
    ChessResult ret = ChessResult::None;
    if ((GetLibertyOf(x, y) == 0)
        || (x > 0 && Chess[x - 1][y] == p && GetLibertyOf(x - 1, y) == 0)
        || (x < Length - 1 && Chess[x + 1][y] == p && GetLibertyOf(x + 1, y) == 0)
        || (y > 0 && Chess[x][y - 1] == p && GetLibertyOf(x, y - 1) == 0)
        || (y < Length - 1 && Chess[x][y + 1] == p && GetLibertyOf(x, y + 1) == 0))
        ret = static_cast<ChessResult>(p);
    const_cast<CNoGoChess*>(this)->Chess[x][y] = Piece::None;
    return ret;
}

int CNoGoChess::GetLibertyOf(int x, int y) const
{
    Piece cur = Chess[x][y];
    if (cur == Piece::None) return -1;
    std::queue<byte> qSearch;
    qSearch.push((x << 4) + y);
    std::vector<std::vector<byte>> SearchTable(Length, std::vector<byte>(Length, 0));
    int Libertys = 0;
    constexpr byte Searched = 1, Liberty = 2;
    auto f = [&](int i, int j)
    {
        auto p = Chess[i][j];
        if (p == Piece::None)
        {
            SearchTable[i][j] = Liberty;
            ++Libertys;
        }
        else if (p == cur && !SearchTable[i][j])
        {
            qSearch.push((i << 4) + j);
            SearchTable[i][j] = Searched;
        }
    };
    while (!qSearch.empty())
    {
        byte b = qSearch.front();
        qSearch.pop();
        int nx = b >> 4, ny = b & 0xf;
        if (nx > 0) f(nx - 1, ny);
        if (nx < Length - 1) f(nx + 1, ny);
        if (ny > 0) f(nx, ny - 1);
        if (ny < Length - 1) f(nx, ny + 1);
    }
    return Libertys;
}

byte CNoGoChess::AIResponse(byte AIType) const
{
    switch (AIType)
    {
    case 0: return CMYAI();
    case 1: return RandomAI();
    case 2: return EvaluateAI();
    default: ASSERT(FALSE);
    }
}

byte CNoGoChess::CMYAI() const
{
    if (Step == 0) return 1;
    int i, j;
    std::vector<std::vector<int>> PosScore(Length, std::vector<int>(Length));
    std::vector<std::vector<Piece>>& chess = const_cast<CNoGoChess*>(this)->Chess;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (!CanPlacePiece(i, j))
                PosScore[i][j] = -10;
            else if (LazyJudge(i, j, Next) == static_cast<ChessResult>(-Next))
                PosScore[i][j] = -1;
            else
            {
                int& score = PosScore[i][j];
                score = 1000;
                if (LazyJudge(i, j, -Next) == static_cast<ChessResult>(Next))
                    score -= 700;

                chess[i][j] = Next;
                if (i > 0
                    && LazyJudge(i - 1, j, Next) == ChessResult::None
                    && LazyJudge(i - 1, j, -Next) == static_cast<ChessResult>(Next))
                    score += 70;
                if (i < Length - 1
                    && LazyJudge(i + 1, j, Next) == ChessResult::None
                    && LazyJudge(i + 1, j, -Next) == static_cast<ChessResult>(Next))
                    score += 70;
                if (j > 0
                    && LazyJudge(i, j - 1, Next) == ChessResult::None
                    && LazyJudge(i, j - 1, -Next) == static_cast<ChessResult>(Next))
                    score += 70;
                if (j < Length - 1
                    && LazyJudge(i, j + 1, Next) == ChessResult::None
                    && LazyJudge(i, j + 1, -Next) == static_cast<ChessResult>(Next))
                    score += 70;
                int lib = GetLibertyOf(i, j);
                score -= lib * 10;
                chess[i][j] = Piece::None;

                if (i > 0 && chess[i - 1][j] == Piece::None)
                {
                    if (j > 0
                        && chess[i][j - 1] == Piece::None
                        && chess[i - 1][j - 1] == Next)
                        score += 18;
                    if (j < Length - 1
                        && chess[i][j + 1] == Piece::None
                        && chess[i - 1][j + 1] == Next)
                        score += 18;
                }
                if (i < Length - 1 && chess[i + 1][j] == Piece::None)
                {
                    if (j > 0
                        && chess[i][j - 1] == Piece::None
                        && chess[i + 1][j - 1] == Next)
                        score += 18;
                    if (j < Length - 1
                        && chess[i][j + 1] == Piece::None
                        && chess[i + 1][j + 1] == Next)
                        score += 18;
                }
            }

    int maxi = 0, maxj = 0;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (PosScore[i][j] > PosScore[maxi][maxj])
            {
                maxi = i;
                maxj = j;
            }

    if (Step < 40 || PosScore[maxi][maxj] > 1025)
    {
        std::vector<byte> possible;
        for (i = 0; i < Length; ++i)
            for (j = 0; j < Length; ++j)
                if (PosScore[maxi][maxj] - PosScore[i][j] < 15)
                    possible.push_back((i << 4) + j);
        std::random_device rd;
        return possible[rd() % possible.size()];
    }
    else
        return EvaluateAI();
}

byte CNoGoChess::RandomAI() const
{
    int i, j;
    std::vector<byte> possible;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (CanPlacePiece(i, j) && LazyJudge(i, j, Next) != static_cast<ChessResult>(-Next))
                possible.push_back((i << 4) + j);
    if (possible.empty())
        for (i = 0; i < Length; ++i)
            for (j = 0; j < Length; ++j)
                if (CanPlacePiece(i, j))
                    return (i << 4) + j;
    std::random_device rd;
    return possible[rd() % possible.size()];
}

int CNoGoChess::Evaluate() const
{
    int res = 0;
    int i, j;
    std::vector<std::vector<Piece>>& chess = const_cast<CNoGoChess*>(this)->Chess;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (CanPlacePiece(i, j))
            {
                if (LazyJudge(i, j, Next) != ChessResult::None)
                    --res;
                if (LazyJudge(i, j, -Next) != ChessResult::None)
                    ++res;
            }
    return res;
}

byte CNoGoChess::EvaluateAI() const
{
    std::vector<std::vector<Piece>>& chess = const_cast<CNoGoChess*>(this)->Chess;
    int i, j, maxscore = -65536;
    std::vector<byte> maxpos;
    for (i = 0; i < Length; ++i)
        for (j = 0; j < Length; ++j)
            if (CanPlacePiece(i, j) && LazyJudge(i, j, Next) == ChessResult::None)
            {
                chess[i][j] = Next;
                int score = Evaluate();
                if (score > maxscore)
                {
                    maxscore = score;
                    maxpos = { static_cast<byte>((i << 4) + j) };
                }
                else if (score == maxscore)
                    maxpos.push_back(static_cast<byte>((i << 4) + j));
                chess[i][j] = Piece::None;
            }

    if (maxscore < -60000)
    {
        for (i = 0; i < Length; ++i)
            for (j = 0; j < Length; ++j)
                if (CanPlacePiece(i, j))
                    return (i << 4) + j;
    }
    std::random_device rd;
    return maxpos[rd() % maxpos.size()];;
}
