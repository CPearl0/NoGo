#pragma once
#include "Chess.h"
class CNoGoChess : public CChess
{
	friend class CNoGoDoc;
protected:
	int GetLibertyOf(int x, int y) const; // ����ĳ����ġ�����
public:
	CNoGoChess(byte length) : CChess(length) {}
	CNoGoChess(const CNoGoChess&) = default;
	CNoGoChess(CNoGoChess&&) = default;
	CNoGoChess& operator=(const CNoGoChess&) = default;
	CNoGoChess& operator=(CNoGoChess&&) = default;
	virtual ~CNoGoChess() = default;

	virtual bool CanPlacePiece(int x, int y) const override; // �жϴ˴��Ƿ���Է�������
	virtual ChessResult LazyJudge(int x, int y, Piece p) const override; // ��x,y�ı�ʱ�ж�ʤ��

	virtual byte AIResponse(byte AIType) const override; // AI������һ����
private:
	byte CMYAI() const;
	byte RandomAI() const;
	byte EvaluateAI() const;
	int Evaluate() const;
};