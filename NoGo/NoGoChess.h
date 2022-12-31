#pragma once
#include "Chess.h"
class CNoGoChess : public CChess
{
	friend class CNoGoDoc;
protected:
	int GetLibertyOf(int x, int y) const; // 计算某块棋的“气”
public:
	CNoGoChess(byte length) : CChess(length) {}
	CNoGoChess(const CNoGoChess&) = default;
	CNoGoChess(CNoGoChess&&) = default;
	CNoGoChess& operator=(const CNoGoChess&) = default;
	CNoGoChess& operator=(CNoGoChess&&) = default;
	virtual ~CNoGoChess() = default;

	virtual bool CanPlacePiece(int x, int y) const override; // 判断此处是否可以放置棋子
	virtual ChessResult LazyJudge(int x, int y, Piece p) const override; // 当x,y改变时判断胜负

	virtual byte AIResponse(byte AIType) const override; // AI给出下一步棋
private:
	byte CMYAI() const;
	byte RandomAI() const;
	byte EvaluateAI() const;
	int Evaluate() const;
};