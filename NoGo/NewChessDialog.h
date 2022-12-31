#pragma once
#include "afxdialogex.h"


// CNewChessDialog 对话框

class CNewChessDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CNewChessDialog)

public:
	CNewChessDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewChessDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEWCHESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_Length;
	int m_ChessType;
	afx_msg void OnBnClickedOk();
};
