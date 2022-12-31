// CNewChessDialog.cpp: 实现文件
//

#include "pch.h"
#include "NoGo.h"
#include "afxdialogex.h"
#include "NewChessDialog.h"


// CNewChessDialog 对话框

IMPLEMENT_DYNAMIC(CNewChessDialog, CDialogEx)

CNewChessDialog::CNewChessDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEWCHESS, pParent)
	, m_Length(9)
	, m_ChessType(0)
{

}

CNewChessDialog::~CNewChessDialog()
{
}

void CNewChessDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_Length);
	DDX_Radio(pDX, IDC_RADIONOGO, m_ChessType);
}


BEGIN_MESSAGE_MAP(CNewChessDialog, CDialogEx)

	ON_BN_CLICKED(IDOK, &CNewChessDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewChessDialog 消息处理程序


void CNewChessDialog::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (5 < m_Length && m_Length < 16)
		CDialogEx::OnOK();
	else
		MessageBox(L"棋盘边长应为6至15!", L"无法创建", MB_ICONERROR);
}
