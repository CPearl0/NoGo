
// NoGoView.h: CNoGoView 类的接口
//

#pragma once
#include "NoGoDoc.h"
#include "OnlinePlayDialog.h"

class CNoGoView : public CView
{
	friend UINT ThreadAIPlay(LPVOID pParam);
	friend UINT ThreadServer(LPVOID pParam);
	friend UINT ThreadClient(LPVOID pParam);
public:
	enum class PlayMode { SelfPlay, AIPlay, OnlinePlay };
private:
	CImage m_imgBoard;
	int m_StartXY = 50, m_ChessSize = 0;
	PlayMode m_Mode = PlayMode::SelfPlay;
	bool m_bMyTurn = true, m_bPause = false, m_bShowStep = false;
	int m_aiType = 0;
	COnlinePlayDialog m_dlgOnlinePlay;
protected: // 仅从序列化创建
	CNoGoView() noexcept;
	DECLARE_DYNCREATE(CNoGoView)

// 特性
public:
	CNoGoDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CNoGoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool PlacePiece(int x, int y);
	void MyPlacePiece(int x, int y); // 用户/AI单步输入

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChessAinext();
	afx_msg void OnChessEnd();
	afx_msg void OnChessNext();
	afx_msg void OnChessPrev();
	afx_msg void OnChessStart();
	afx_msg void OnChessAiplay();
protected:
	afx_msg LRESULT OnPlacePiece(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnUpdateChessAiplay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChessAinext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChessStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChessNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChessPrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChessEnd(CCmdUI* pCmdUI);
	afx_msg void OnChessReset();
	afx_msg void OnUpdateChessReset(CCmdUI* pCmdUI);
	afx_msg void OnNoGoAICMY();
	afx_msg void OnNoGoAIRandom();
	afx_msg void OnNoGoAIEvaluate();
	afx_msg void OnGobangAIRandom();
	afx_msg void OnUpdateNoGoAICMY(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNoGoAIRandom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNoGoAIEvaluate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGobangAIRandom(CCmdUI* pCmdUI);
	afx_msg void OnOnlinePlay();
	afx_msg void OnUpdateOnlinePlay(CCmdUI* pCmdUI);
	afx_msg void OnChessPause();
	afx_msg void OnUpdateChessPause(CCmdUI* pCmdUI);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnViewShowStep();
	afx_msg void OnUpdateViewShowStep(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // NoGoView.cpp 中的调试版本
inline CNoGoDoc* CNoGoView::GetDocument() const
   { return reinterpret_cast<CNoGoDoc*>(m_pDocument); }
#endif

