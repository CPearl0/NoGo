// OnlinePlayDialog.cpp: 实现文件
//

#include "pch.h"
#include "NoGo.h"
#include "afxdialogex.h"
#include "OnlinePlayDialog.h"
#include "GobangChess.h"
#include "NoGoView.h"
#include "NoGoDoc.h"

// COnlinePlayDialog 对话框
#define WM_CONNECTEDSUCCESSFULLY WM_USER + 102
#define WM_CONNECTEND WM_USER + 103

IMPLEMENT_DYNAMIC(COnlinePlayDialog, CDialogEx)

COnlinePlayDialog::COnlinePlayDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ONLINEPLAY, pParent)
	, m_strIP(_T("127.0.0.1"))
	, m_uPort(8000)
	, m_pView(nullptr)
	, m_ServerBlack(0)
{

}

COnlinePlayDialog::~COnlinePlayDialog()
{
}

void COnlinePlayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_uPort);
	DDX_Radio(pDX, IDC_RADIO_BLACK, m_ServerBlack);
}


BEGIN_MESSAGE_MAP(COnlinePlayDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SERVER, &COnlinePlayDialog::OnBnClickedButtonServer)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT, &COnlinePlayDialog::OnBnClickedButtonClient)
	ON_MESSAGE(WM_CONNECTEDSUCCESSFULLY, &COnlinePlayDialog::OnConnectedSuccessfully)
	ON_MESSAGE(WM_CONNECTEND, &COnlinePlayDialog::OnConnectEnd)
END_MESSAGE_MAP()


void COnlinePlayDialog::EnableAll(BOOL b)
{
	GetDlgItem(IDC_EDIT_ADDRESS)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(b);
	GetDlgItem(IDC_RADIO_BLACK)->EnableWindow(b);
	GetDlgItem(IDC_RADIO_WHITE)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_SERVER)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_CLIENT)->EnableWindow(b);
	GetDlgItem(IDCANCEL)->EnableWindow(b);
}


// COnlinePlayDialog 消息处理程序

// 服务器端线程
UINT ThreadServer(LPVOID pParam)
{
	COnlinePlayDialog* pDlg = reinterpret_cast<COnlinePlayDialog*>(pParam);
	CNoGoView* pView = pDlg->m_pView;
	CNoGoDoc* pDoc = pView->GetDocument();
	pView->m_Mode = CNoGoView::PlayMode::OnlinePlay;
	pView->m_bMyTurn = false;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		pDlg->SendMessage(WM_CONNECTEDSUCCESSFULLY);
		return FALSE;
	}
	CSocket sockServ, sockClient;
	sockServ.Create(pDlg->m_uPort);
	sockServ.Listen(1);
	sockServ.Accept(sockClient);
	byte ChessType;
	if (typeid(*pDoc->m_pChess.get()) == typeid(CNoGoChess))
		ChessType = CT_NOGO;
	else if (typeid(*pDoc->m_pChess.get()) == typeid(CGobangChess))
		ChessType = CT_GOBANG;
	sockClient.Send(&ChessType, sizeof(byte));
	sockClient.Send(&pDoc->m_pChess->Length, sizeof(byte));
	sockClient.Send(&pDoc->m_pChess->Step, sizeof(int));
	for (byte tmp : pDoc->m_pChess->Record)
		sockClient.Send(&tmp, sizeof(byte));
	bool isBlack = !pDlg->m_ServerBlack;
	pView->m_bMyTurn = isBlack ^ (pDoc->m_pChess->Next == Piece::White);
	sockClient.Send(&isBlack, sizeof(bool));
	pDlg->SendMessage(WM_CONNECTEDSUCCESSFULLY);

	byte tmp;
	if (pView->m_bMyTurn)
	{
		while (pView->m_bMyTurn); // 等待输入
		tmp = pDoc->m_pChess->GetLast();
		sockClient.Send(&tmp, sizeof(byte));
		Sleep(100); // 等待主程序判断出Result
	}
	while (pDoc->m_pChess->Result == ChessResult::None)
	{
		sockClient.Receive(&tmp, sizeof(byte));
		pView->SendMessage(WM_PLACEPIECE, tmp);
		Sleep(100); // 等待主程序判断出Result
		if (pDoc->m_pChess->Result == ChessResult::None)
		{
			while (pView->m_bMyTurn); // 等待输入
			tmp = pDoc->m_pChess->GetLast();
			sockClient.Send(&tmp, sizeof(byte));
			Sleep(100); // 等待主程序判断出Result
		}
	}
	tmp = 'E';
	sockClient.Send(&tmp, sizeof(byte));
	sockClient.Receive(&tmp, sizeof(byte));
	ASSERT(tmp == 'E');
	Sleep(2000);
	sockClient.Close();
	pDlg->SendMessage(WM_CONNECTEND);
	return 0;
}

void COnlinePlayDialog::OnBnClickedButtonServer()
{
	UpdateData();
	AfxBeginThread(ThreadServer, this);
	EnableAll(FALSE);
}

UINT ThreadClient(LPVOID pParam)
{
	COnlinePlayDialog* pDlg = reinterpret_cast<COnlinePlayDialog*>(pParam);
	CNoGoView* pView = pDlg->m_pView;
	CNoGoDoc* pDoc = pView->GetDocument();
	pView->m_Mode = CNoGoView::PlayMode::OnlinePlay;
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		pDlg->SendMessage(WM_CONNECTEDSUCCESSFULLY);
		return FALSE;
	}

	CSocket sockClient;
	sockClient.Create();
	if (!sockClient.Connect(pDlg->m_strIP, pDlg->m_uPort))
		throw 0;
	byte ChessType, Length;
	sockClient.Receive(&ChessType, sizeof(byte));
	sockClient.Receive(&Length, sizeof(byte));
	switch (ChessType)
	{
	case CT_NOGO:
		pDoc->m_pChess.reset(new CNoGoChess(Length));
		break;
	case CT_GOBANG:
		pDoc->m_pChess.reset(new CGobangChess(Length));
		break;
	}
	sockClient.Receive(&pDoc->m_pChess->Step, sizeof(int));
	bool black = true;
	for (int i = 0; i < pDoc->m_pChess->Step; ++i)
	{
		byte b;
		sockClient.Receive(&b, sizeof(byte));
		pDoc->m_pChess->Chess[b >> 4][b & 0xf] = black ? Piece::Black : Piece::White;
		pDoc->m_pChess->Record.push_back(b);
		black = !black;
	}
	pDoc->m_pChess->Next = black ? Piece::Black : Piece::White;
	pView->Invalidate();
	bool isBlack;
	sockClient.Receive(&isBlack, sizeof(bool));
	isBlack = !isBlack;
	pView->m_bMyTurn = isBlack ^ (pDoc->m_pChess->Next == Piece::White);
	pDlg->SendMessage(WM_CONNECTEDSUCCESSFULLY);

	byte tmp;
	if (pView->m_bMyTurn)
	{
		while (pView->m_bMyTurn); // 等待输入
		tmp = pDoc->m_pChess->GetLast();
		sockClient.Send(&tmp, sizeof(byte));
		Sleep(100); // 等待主程序判断出Result
	}
	while (pDoc->m_pChess->Result == ChessResult::None)
	{
		sockClient.Receive(&tmp, sizeof(byte));
		pView->SendMessage(WM_PLACEPIECE, tmp);
		Sleep(100); // 等待主程序判断出Result
		if (pDoc->m_pChess->Result == ChessResult::None)
		{
			while (pView->m_bMyTurn); // 等待输入
			tmp = pDoc->m_pChess->GetLast();
			sockClient.Send(&tmp, sizeof(byte));
			Sleep(100); // 等待主程序判断出Result
		}
	}
	sockClient.Receive(&tmp, sizeof(byte));
	ASSERT(tmp == 'E');
	tmp = 'E';
	sockClient.Send(&tmp, sizeof(byte));
	Sleep(2000);
	sockClient.Close();
	pDlg->SendMessage(WM_CONNECTEND);
	return 0;
}

void COnlinePlayDialog::OnBnClickedButtonClient()
{
	UpdateData();
	AfxBeginThread(ThreadClient, this);
	EnableAll(FALSE);
}


BOOL COnlinePlayDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow(m_pView); // 使窗口居中

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT COnlinePlayDialog::OnConnectedSuccessfully(WPARAM wParam, LPARAM lParam)
{
	OnOK();
	return 0;
}


afx_msg LRESULT COnlinePlayDialog::OnConnectEnd(WPARAM wParam, LPARAM lParam)
{
	EnableAll(TRUE);
	return 0;
}
