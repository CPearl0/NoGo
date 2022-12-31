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

	virtual ChessResult LazyJudge(int x, int y, Piece p) const override; // ��x,y�ı�ʱ�ж�ʤ��

	virtual byte AIResponse(byte AIType) const override; // AI������һ����
private:
	byte SimpleRandomAI() const;
};