#pragma once
#include <vector>
#include "Chess.h"
class CGobangChess : public CChess
{
	friend class CNoGoDoc;
public:
	CGobangChess(byte length) : CChess(length) {}
	CGobangChess(const CGobangChess&) = default;
	CGobangChess(CGobangChess&&) = default;
	CGobangChess& operator=(const CGobangChess&) = default;
	CGobangChess& operator=(CGobangChess&&) = default;
	virtual ~CGobangChess() = default;

	virtual ChessResult LazyJudge(int x, int y, Piece p) const override; // 当x,y改变时判断胜负

	virtual byte AIResponse(byte AIType) const override; // AI给出下一步棋
private:
	byte SimpleRandomAI() const;
};