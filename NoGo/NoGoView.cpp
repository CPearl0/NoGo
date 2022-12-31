
// NoGoView.cpp: CNoGoView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "NoGo.h"
#endif

#include "NoGoDoc.h"
#include "NoGoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MainFrm.h"
#include "GobangChess.h"
#include "OnlinePlayDialog.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
// CNoGoView

IMPLEMENT_DYNCREATE(CNoGoView, CView)

BEGIN_MESSAGE_MAP(CNoGoView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CNoGoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_CHESS_AINEXT, &CNoGoView::OnChessAinext)
	ON_COMMAND(ID_CHESS_END, &CNoGoView::OnChessEnd)
	ON_COMMAND(ID_CHESS_NEXT, &CNoGoView::OnChessNext)
	ON_COMMAND(ID_CHESS_PREV, &CNoGoView::OnChessPrev)
	ON_COMMAND(ID_CHESS_START, &CNoGoView::OnChessStart)
	ON_COMMAND(ID_CHESS_AIPLAY, &CNoGoView::OnChessAiplay)
	ON_MESSAGE(WM_PLACEPIECE, &CNoGoView::OnPlacePiece)
	ON_UPDATE_COMMAND_UI(ID_CHESS_AIPLAY, &CNoGoView::OnUpdateChessAiplay)
	ON_UPDATE_COMMAND_UI(ID_CHESS_AINEXT, &CNoGoView::OnUpdateChessAinext)
	ON_UPDATE_COMMAND_UI(ID_CHESS_START, &CNoGoView::OnUpdateChessStart)
	ON_UPDATE_COMMAND_UI(ID_CHESS_NEXT, &CNoGoView::OnUpdateChessNext)
	ON_UPDATE_COMMAND_UI(ID_CHESS_PREV, &CNoGoView::OnUpdateChessPrev)
	ON_UPDATE_COMMAND_UI(ID_CHESS_END, &CNoGoView::OnUpdateChessEnd)
	ON_COMMAND(ID_CHESS_RESET, &CNoGoView::OnChessReset)
	ON_UPDATE_COMMAND_UI(ID_CHESS_RESET, &CNoGoView::OnUpdateChessReset)
	ON_COMMAND(ID_NOGOAI_CMY, &CNoGoView::OnNoGoAICMY)
	ON_COMMAND(ID_NOGOAI_RANDOM, &CNoGoView::OnNoGoAIRandom)
	ON_COMMAND(ID_NOGOAI_EVALUATE, &CNoGoView::OnNoGoAIEvaluate)
	ON_COMMAND(ID_GOBANGAI_RANDOM, &CNoGoView::OnGobangAIRandom)
	ON_UPDATE_COMMAND_UI(ID_NOGOAI_CMY, &CNoGoView::OnUpdateNoGoAICMY)
	ON_UPDATE_COMMAND_UI(ID_NOGOAI_RANDOM, &CNoGoView::OnUpdateNoGoAIRandom)
	ON_UPDATE_COMMAND_UI(ID_NOGOAI_EVALUATE, &CNoGoView::OnUpdateNoGoAIEvaluate)
	ON_UPDATE_COMMAND_UI(ID_GOBANGAI_RANDOM, &CNoGoView::OnUpdateGobangAIRandom)
	ON_COMMAND(ID_ONLINEPLAY, &CNoGoView::OnOnlinePlay)
	ON_UPDATE_COMMAND_UI(ID_ONLINEPLAY, &CNoGoView::OnUpdateOnlinePlay)
	ON_COMMAND(ID_CHESS_PAUSE, &CNoGoView::OnChessPause)
	ON_UPDATE_COMMAND_UI(ID_CHESS_PAUSE, &CNoGoView::OnUpdateChessPause)
	ON_COMMAND(ID_VIEW_SHOWSTEP, &CNoGoView::OnViewShowStep)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWSTEP, &CNoGoView::OnUpdateViewShowStep)
END_MESSAGE_MAP()

// CNoGoView 构造/析构

CNoGoView::CNoGoView() noexcept
	: m_Mode(PlayMode::SelfPlay),
	m_dlgOnlinePlay(this)
{
	m_imgBoard.LoadFromResource(AfxGetInstanceHandle(), IDB_BMPBOARD);
	m_dlgOnlinePlay.Create(IDD_ONLINEPLAY);
	m_dlgOnlinePlay.m_pView = this;
}

CNoGoView::~CNoGoView()
{
	m_dlgOnlinePlay.DestroyWindow();
}

BOOL CNoGoView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CNoGoView 绘图

void CNoGoView::OnDraw(CDC* pDC)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pChess)
		return;
	int l = pDoc->m_pChess->Length;
	bool bPrinting = pDC->IsPrinting(); // 如果是Print,则需要颠倒y轴方向输出
	m_ChessSize = (500 - m_StartXY) / l;

	// 绘制棋盘材质
	m_imgBoard.Draw(pDC->GetSafeHdc(), 0, 0);

	// 绘制棋盘格
	CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	int i, j, t;
	for (i = 0; i < l; ++i)
	{
		pDC->MoveTo(m_StartXY, m_ChessSize * i + m_StartXY);
		pDC->LineTo(m_StartXY + (l - 1) * m_ChessSize, m_ChessSize * i + m_StartXY);
		pDC->MoveTo(m_ChessSize * i + m_StartXY, m_StartXY);
		pDC->LineTo(m_ChessSize * i + m_StartXY, m_StartXY + (l - 1) * m_ChessSize);
	}

	if (pDoc->m_pChess->GetCurStep())
	{
		CBrush brBlack(RGB(0, 0, 0)), brWhite(RGB(255, 255, 255)), brSign(RGB(255, 64, 64));

		// 绘制最后一步的红点
		auto DrawLastRedRect = [&](int i, int j)
		{
			pDC->SelectObject(&brSign);
			pDC->Rectangle(m_StartXY + i * m_ChessSize - m_ChessSize / 8,
				m_StartXY + j * m_ChessSize - m_ChessSize / 8,
				m_StartXY + i * m_ChessSize + m_ChessSize / 8,
				m_StartXY + j * m_ChessSize + m_ChessSize / 8);
		};

		// 绘制棋子
		size_t sz = pDoc->m_pChess->GetCurStep();
		CFont font;
		VERIFY(font.CreatePointFont(170, _T("Times New Roman"), pDC));
		CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0)), penWhite(PS_SOLID, 1, RGB(255, 255, 255));
		CFont* pOldFont = pDC->SelectObject(&font);
		byte b;
		CString str;
		pDC->SetBkMode(TRANSPARENT);
		// 黑棋
		CBrush* pOldBrush = pDC->SelectObject(&brBlack);
		pDC->SelectObject(&penWhite);
		pDC->SetTextColor(RGB(255, 255, 255));
		for (t = 0; t < sz; t += 2)
		{
			b = pDoc->m_pChess->GetRecordAt(t);
			i = b >> 4;
			j = b & 0xf;
			j = bPrinting ? (l - 1 - j) : j;
			CRect rc(m_StartXY + i * m_ChessSize - m_ChessSize / 2,
				m_StartXY + j * m_ChessSize - m_ChessSize / 2,
				m_StartXY + i * m_ChessSize + m_ChessSize / 2,
				m_StartXY + j * m_ChessSize + m_ChessSize / 2);
			pDC->Ellipse(&rc);
			if (t == sz - 1) DrawLastRedRect(i, j);
			if (m_bShowStep)
			{
				str.Format(_T("%d"), t + 1);
				pDC->DrawText(str, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		// 白棋
		pDC->SelectObject(&brWhite);
		pDC->SelectObject(&penBlack);
		pDC->SetTextColor(RGB(0, 0, 0));
		for (t = 1; t < sz; t += 2)
		{
			b = pDoc->m_pChess->GetRecordAt(t);
			i = b >> 4;
			j = b & 0xf;
			j = bPrinting ? (l - 1 - j) : j;
			CRect rc(m_StartXY + i * m_ChessSize - m_ChessSize / 2,
				m_StartXY + j * m_ChessSize - m_ChessSize / 2,
				m_StartXY + i * m_ChessSize + m_ChessSize / 2,
				m_StartXY + j * m_ChessSize + m_ChessSize / 2);
			pDC->Ellipse(&rc);
			if (t == sz - 1) DrawLastRedRect(i, j);
			if (m_bShowStep)
			{
				str.Format(_T("%d"), t + 1);
				pDC->DrawText(str, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldFont);
	}
	pDC->SelectObject(pOldPen);
}


// CNoGoView 打印


void CNoGoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CNoGoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CNoGoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
}

void CNoGoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}


void CNoGoView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if (pDC->IsPrinting())
	{
		pDC->SetMapMode(MM_LOENGLISH);
		if (pInfo->m_bPreview)
		{
			pDC->OffsetWindowOrg(-100, 100);
			pDC->OffsetWindowOrg(0, 500);
		}
		else
		{
			pDC->OffsetWindowOrg(-50, 50);
			pDC->OffsetWindowOrg(0, 250);
		}
	}
	CView::OnPrepareDC(pDC, pInfo);
}

void CNoGoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CNoGoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CNoGoView 诊断

#ifdef _DEBUG
void CNoGoView::AssertValid() const
{
	CView::AssertValid();
}

void CNoGoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNoGoDoc* CNoGoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNoGoDoc)));
	return (CNoGoDoc*)m_pDocument;
}
#endif //_DEBUG

// 保证CanPlacePiece,此函数放置棋子,播放音频,并显示
bool CNoGoView::PlacePiece(int x, int y)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	bool b = false;
	if (pDoc)
	{
		byte la = pDoc->m_pChess->GetLast();
		int lx = la >> 4, ly = la & 0xf;
		b = pDoc->m_pChess->PlacePiece(x, y);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE_PLACEPIECE), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
		CRect rc(m_StartXY + x * m_ChessSize - m_ChessSize / 2,
			m_StartXY + y * m_ChessSize - m_ChessSize / 2,
			m_StartXY + x * m_ChessSize + m_ChessSize / 2,
			m_StartXY + y * m_ChessSize + m_ChessSize / 2);
		InvalidateRect(&rc);
		rc = CRect(m_StartXY + lx * m_ChessSize - m_ChessSize / 8,
			m_StartXY + ly * m_ChessSize - m_ChessSize / 8,
			m_StartXY + lx * m_ChessSize + m_ChessSize / 8,
			m_StartXY + ly * m_ChessSize + m_ChessSize / 8);
		InvalidateRect(&rc);
	}
	return b;
}

// 保证CanPlacePiece,此函数负责用户或者ai代替用户进行的放置棋子操作
void CNoGoView::MyPlacePiece(int x, int y)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	bool b = false;
	if (pDoc)
	{
		PlacePiece(x, y);
		if (m_Mode != PlayMode::SelfPlay) m_bMyTurn = false;
		switch (pDoc->m_pChess->GetResult())
		{
		case ChessResult::Black:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("黑胜!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		case ChessResult::White:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("白胜!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		case ChessResult::Draw:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("平局!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		}
	}
}

// CNoGoView 消息处理程序


void CNoGoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!m_bPause && pDoc)
	{
		if (pDoc->m_pChess->GetNext() != Piece::None && m_bMyTurn)
		{
			int i, j, l = pDoc->m_pChess->Length;
			for (i = 0; i < l; ++i)
				for (j = 0; j < l; ++j)
					if (point.x > m_StartXY + i * m_ChessSize - m_ChessSize / 3
						&& point.x < m_StartXY + i * m_ChessSize + m_ChessSize / 3
						&& point.y > m_StartXY + j * m_ChessSize - m_ChessSize / 3
						&& point.y < m_StartXY + j * m_ChessSize + m_ChessSize / 3)
						goto outbreak0;
			outbreak0:
			if (pDoc->m_pChess->CanPlacePiece(i, j))
				MyPlacePiece(i, j);
		}
	}
	CView::OnLButtonUp(nFlags, point);
}


void CNoGoView::OnChessNext()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		byte old = pDoc->m_pChess->GetLast();
		pDoc->m_pChess->GoNext();
		byte ne = pDoc->m_pChess->GetLast();
		int ox = old >> 4, oy = old & 0xf, nx = ne >> 4, ny = ne & 0xf;
		CRect rc(m_StartXY + nx * m_ChessSize - m_ChessSize / 2,
			m_StartXY + ny * m_ChessSize - m_ChessSize / 2,
			m_StartXY + nx * m_ChessSize + m_ChessSize / 2,
			m_StartXY + ny * m_ChessSize + m_ChessSize / 2);
		InvalidateRect(&rc);
		rc = CRect(m_StartXY + ox * m_ChessSize - m_ChessSize / 8,
			m_StartXY + oy * m_ChessSize - m_ChessSize / 8,
			m_StartXY + ox * m_ChessSize + m_ChessSize / 8,
			m_StartXY + oy * m_ChessSize + m_ChessSize / 8);
		InvalidateRect(&rc);
	}
}


void CNoGoView::OnChessPrev()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		byte old = pDoc->m_pChess->GetLast();
		pDoc->m_pChess->GoPrev();
		byte ne = pDoc->m_pChess->GetLast();
		int ox = old >> 4, oy = old & 0xf, nx = ne >> 4, ny = ne & 0xf;
		CRect rc(m_StartXY + ox * m_ChessSize - m_ChessSize / 2,
			m_StartXY + oy * m_ChessSize - m_ChessSize / 2,
			m_StartXY + ox * m_ChessSize + m_ChessSize / 2,
			m_StartXY + oy * m_ChessSize + m_ChessSize / 2);
		InvalidateRect(&rc);
		rc = CRect(m_StartXY + nx * m_ChessSize - m_ChessSize / 8,
			m_StartXY + ny * m_ChessSize - m_ChessSize / 8,
			m_StartXY + nx * m_ChessSize + m_ChessSize / 8,
			m_StartXY + ny * m_ChessSize + m_ChessSize / 8);
		InvalidateRect(&rc);
	}
}


void CNoGoView::OnChessStart()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		pDoc->m_pChess->GoStart();
		Invalidate();
	}
}


void CNoGoView::OnChessEnd()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		pDoc->m_pChess->GoEnd();
		Invalidate();
	}
}


void CNoGoView::OnChessReset()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		pDoc->m_pChess->Reset();
		Invalidate();
	}
}


void CNoGoView::OnChessAinext()
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc && m_bMyTurn)
	{
		byte b = pDoc->m_pChess->AIResponse(m_aiType);
		MyPlacePiece(b >> 4, b & 0xf);
	}
}


UINT ThreadAIPlay(LPVOID pParam)
{
	CNoGoView* view = (CNoGoView*)pParam;
	while (view->m_Mode == CNoGoView::PlayMode::AIPlay)
	{
		if (!view->m_bMyTurn && view->GetDocument()->m_pChess->GetResult() == ChessResult::None)
		{
			byte b = ((CNoGoDoc*)(view->GetDocument()))->m_pChess->AIResponse(view->m_aiType);
			view->SendMessage(WM_PLACEPIECE, b);
			Sleep(100);
		}
	}
	return 0;
}

void CNoGoView::OnChessAiplay()
{
	if (m_Mode == PlayMode::AIPlay)
	{
		m_Mode = PlayMode::SelfPlay;
		m_bMyTurn = true;
	}
	else
	{
		m_Mode = PlayMode::AIPlay;
		m_bMyTurn = false;
		AfxBeginThread(ThreadAIPlay, this);
	}
}

// 应保证CanPlacePiece = true,此函数负责ai或者联机对战的对手进行的放置棋子操作
afx_msg LRESULT CNoGoView::OnPlacePiece(WPARAM wParam, LPARAM lParam)
{
	byte b = static_cast<byte>(wParam);
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		PlacePiece(b >> 4, b & 0xf);
		m_bMyTurn = true;
		switch (pDoc->m_pChess->GetResult())
		{
		case ChessResult::Black:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("黑胜!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		case ChessResult::White:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("白胜!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		case ChessResult::Draw:
			m_Mode = PlayMode::SelfPlay;
			MessageBox(_T("平局!"), _T("对局结束"), MB_ICONINFORMATION);
			break;
		}
	}
	return 0;
}


void CNoGoView::OnUpdateChessAiplay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_Mode == PlayMode::AIPlay);
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() == pDoc->m_pChess->GetRecordSize()
			&& pDoc->m_pChess->GetNext() != Piece::None
			&& m_Mode != PlayMode::OnlinePlay);
}



void CNoGoView::OnUpdateChessAinext(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() == pDoc->m_pChess->GetRecordSize()
			&& pDoc->m_pChess->GetNext() != Piece::None);
}


void CNoGoView::OnUpdateChessStart(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() != 0
			&& m_Mode != PlayMode::OnlinePlay);
}


void CNoGoView::OnUpdateChessPrev(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() != 0
			&& m_Mode != PlayMode::OnlinePlay);
}


void CNoGoView::OnUpdateChessNext(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() != pDoc->m_pChess->GetRecordSize()
			&& m_Mode != PlayMode::OnlinePlay);
}


void CNoGoView::OnUpdateChessEnd(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() != pDoc->m_pChess->GetRecordSize()
			&& m_Mode != PlayMode::OnlinePlay);
}


void CNoGoView::OnUpdateChessReset(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetCurStep() != pDoc->m_pChess->GetRecordSize()
			&& m_Mode == PlayMode::SelfPlay);
}


void CNoGoView::OnNoGoAICMY()
{
	m_aiType = 0;
}


void CNoGoView::OnUpdateNoGoAICMY(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(typeid(*pDoc->m_pChess.get()) == typeid(CNoGoChess));
	pCmdUI->SetRadio(m_aiType == 0);
}


void CNoGoView::OnNoGoAIRandom()
{
	m_aiType = 1;
}


void CNoGoView::OnUpdateNoGoAIRandom(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(typeid(*pDoc->m_pChess.get()) == typeid(CNoGoChess));
	pCmdUI->SetRadio(m_aiType == 1);
}


void CNoGoView::OnNoGoAIEvaluate()
{
	m_aiType = 2;
}


void CNoGoView::OnUpdateNoGoAIEvaluate(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(typeid(*pDoc->m_pChess.get()) == typeid(CNoGoChess));
	pCmdUI->SetRadio(m_aiType == 2);
}


void CNoGoView::OnGobangAIRandom()
{
	m_aiType = 0;
}


void CNoGoView::OnUpdateGobangAIRandom(CCmdUI* pCmdUI)
{
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(typeid(*pDoc->m_pChess.get()) == typeid(CGobangChess));
	pCmdUI->SetRadio(m_aiType == 0);
}


void CNoGoView::OnOnlinePlay()
{
	m_dlgOnlinePlay.ShowWindow(SW_SHOW);
}


void CNoGoView::OnUpdateOnlinePlay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_Mode == PlayMode::OnlinePlay);
	CNoGoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
		pCmdUI->Enable(pDoc->m_pChess->GetResult() == ChessResult::None
			&& pDoc->m_pChess->GetCurStep() == pDoc->m_pChess->GetRecordSize()
			&& m_Mode == PlayMode::SelfPlay);
}


void CNoGoView::OnChessPause()
{
	m_bPause = !m_bPause;
}


void CNoGoView::OnUpdateChessPause(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bPause);
}


void CNoGoView::OnViewShowStep()
{
	m_bShowStep = !m_bShowStep;
	Invalidate();
}


void CNoGoView::OnUpdateViewShowStep(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowStep);
}
