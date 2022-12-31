#pragma once
#include "afxdialogex.h"

#define WM_PLACEPIECE WM_USER + 103
class CNoGoView;

// COnlinePlayDialog 对话框

class COnlinePlayDialog : public CDialogEx
{
	friend UINT ThreadServer(LPVOID pParam);
	friend UINT ThreadClient(LPVOID pParam);
	DECLARE_DYNAMIC(COnlinePlayDialog)

public:
	COnlinePlayDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COnlinePlayDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ONLINEPLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonServer();
	afx_msg void OnBnClickedButtonClient();
	CString m_strIP;
	UINT m_uPort;
	CNoGoView* m_pView;
	int m_ServerBlack;
protected:
	afx_msg LRESULT OnConnectedSuccessfully(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
private:
	void EnableAll(BOOL b);
protected:
	afx_msg LRESULT OnConnectEnd(WPARAM wParam, LPARAM lParam);
};
