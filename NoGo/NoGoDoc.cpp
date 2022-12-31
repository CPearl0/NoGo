
// NoGoDoc.cpp: CNoGoDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "NoGo.h"
#endif

#include "NoGoDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "NewChessDialog.h"
#include "GobangChess.h"

// CNoGoDoc

IMPLEMENT_DYNCREATE(CNoGoDoc, CDocument)

BEGIN_MESSAGE_MAP(CNoGoDoc, CDocument)
END_MESSAGE_MAP()


// CNoGoDoc 构造/析构

CNoGoDoc::CNoGoDoc() noexcept
{
	// TODO: 在此添加一次性构造代码

}

CNoGoDoc::~CNoGoDoc()
{
}

BOOL CNoGoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CNewChessDialog ncdlg;
	if (ncdlg.DoModal() != IDOK) return FALSE;
	switch (ncdlg.m_ChessType)
	{
	case 0:
		m_pChess = std::make_unique<CNoGoChess>(ncdlg.m_Length);
		break;
	case 1:
		m_pChess = std::make_unique<CGobangChess>(ncdlg.m_Length);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}




// CNoGoDoc 序列化

void CNoGoDoc::Serialize(CArchive& ar)
{ 
	if (ar.IsStoring())
	{
		byte ChessType;
		if (typeid(*m_pChess.get()) == typeid(CNoGoChess))
			ChessType = CT_NOGO;
		else if (typeid(*m_pChess.get()) == typeid(CGobangChess))
			ChessType = CT_GOBANG;
		ar << ChessType;
		ar << m_pChess->Length;
		ar << (char)m_pChess->Result;
		int sz = m_pChess->Record.size();
		ar << sz;
		for (byte b : m_pChess->Record)
			ar << b;
	}
	else
	{
		byte ChessType, Length;
		ar >> ChessType >> Length;
		switch (ChessType)
		{
		case CT_NOGO:
			m_pChess.reset(new CNoGoChess(Length));
			break;
		case CT_GOBANG:
			m_pChess.reset(new CGobangChess(Length));
			break;
		}
		char c;
		ar >> c;
		m_pChess->Result = (ChessResult)c;
		int sz;
		ar >> sz;
		m_pChess->Record.clear();
		bool black = true;
		for (int i = 0; i < sz; ++i)
		{
			byte b;
			ar >> b;
			m_pChess->Chess[b >> 4][b & 0xf] = black ? Piece::Black : Piece::White;
			m_pChess->Record.push_back(b);
			black = !black;
		}
		m_pChess->Next = c ? Piece::None : (black ? Piece::Black : Piece::White);
		m_pChess->Step = sz;
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CNoGoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CNoGoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CNoGoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNoGoDoc 诊断

#ifdef _DEBUG
void CNoGoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNoGoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNoGoDoc 命令
