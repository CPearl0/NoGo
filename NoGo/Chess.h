#pragma once
#include <vector>

enum class Piece : char { Black = -1, None = 0, White = 1 };
enum class ChessResult : char { Black = -1, None = 0, White = 1, Draw };
Piece operator-(Piece p);
class CChess abstract
{
	friend class CNoGoDoc;
	friend UINT ThreadServer(LPVOID pParam);
	friend UINT ThreadClient(LPVOID pParam);
protected:
	std::vector<std::vector<Piece>> Chess;
	Piece Next;
	ChessResult Result;
	std::vector<byte> Record; // 高4位表示x 低4位表示y
	int Step;
public:
	CChess(byte l) : Length(l), Chess(l, std::vector<Piece>(l, Piece::None)), Next(Piece::Black), Result(ChessResult::None), Step(0) {}
	CChess(const CChess&) = default;
	CChess(CChess&&) = default;
	CChess& operator=(const CChess&) = default;
	CChess& operator=(CChess&&) = default;
	virtual ~CChess() = default;

	const byte Length;
	Piece At(int x, int y) const { return Chess[x][y]; }
	Piece GetNext() const { return Next; }
	ChessResult GetResult() const { return Result; }
	byte GetLast() const { return Step == 0 ? 255 : Record[Step - 1]; }
	int GetCurStep() const { return Step; }
	size_t GetRecordSize() const { return Record.size(); }
	byte GetRecordAt(size_t index) const { return Record[index]; }
	byte operator[](size_t index) const { return GetRecordAt(index); }

	virtual bool CanPlacePiece(int x, int y) const; // 判断此处是否可以放置棋子
	virtual bool PlacePiece(int x, int y); // 若对局结束则返回true， 请保证CanPlacePiece(x,y) = true
	virtual ChessResult LazyJudge(int x, int y, Piece p) const = 0; // 当x,y改变时判断胜负

	bool GoNext(); // 返回是否到达起始状态
	bool GoPrev(); // 返回是否到达最后
	void GoStart();
	void GoEnd();
	void Reset(); // 将目前Step之后的步骤删除

	virtual byte AIResponse(byte AIType) const = 0; // AI给出下一步棋
};