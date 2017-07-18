/* 2017 (C) Software source, Copyright Owner Shirley Chung. joniesg@gmail.com' */
#pragma once
#include "afxwin.h"
#include "afxtempl.h"

typedef unsigned long ULONG_PTR;
typedef ULONG_PTR DWORD_PTR;

size_t _StringToUNICODE(const CString&, BYTE* buffer);

/*
	Dialog Items
*/
struct CDlgItem
{
	enum	controltype { wndCLASS = -1, BUTTON = 0x0080, EDITCONTROL, STATICTEXT, LISTBOX, SCROLLBAR, COMBOX};

	DLGITEMTEMPLATE m_dlgItem;
	CString m_strCaption;
	controltype m_controlType;
	CString m_wndClass;
	size_t m_szData;
	BYTE* m_pData;

	size_t GetSize()
	{
		size_t sz = sizeof(DLGITEMTEMPLATE) + 3 * sizeof(WCHAR) + (m_strCaption.GetLength() + 1) * sizeof(WCHAR);
		return (sz + 3) & ~3; //DWORD aligned.
	}
	BYTE* WriteToBuffer(BYTE* pBuffer);

	CDlgItem():m_szData(0), m_pData(NULL){};
	~CDlgItem(){ if(m_szData) delete[] m_pData; }
};

/* 
	Resourceless Dialog. chung_shirley
*/
class CSettingDialog : protected CDialog
{
	CList<CDlgItem, CDlgItem>  m_dlgItemList;

	BYTE* _InitDlgTemplate(BYTE* pBuffer, CRect& rt, const CString& strCaption, const CString& fontname, short fontsize, DWORD style, DWORD exStyle);
protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CSettingDialog)
	//}}AFX_MSG

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingDialog)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void AddCheckBox(WORD id, CString caption, CRect rt);
	void AddButton(WORD id, CString caption, CRect rt);
	void AddListCtl(WORD id, CRect rt, DWORD style = WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, DWORD exstyle = 0);

	void AddControl(WORD id, CDlgItem::controltype, DWORD wndStyle, DWORD exStyle, CString caption, short x, short y, short cx, short cy);
	void AddControl(WORD id, CDlgItem::controltype, DWORD wndStyle, DWORD exStyle, CString caption, CRect rt);
	void AddControl(WORD id, CString wndClass, DWORD wndStyle, DWORD exStyle, CString caption, CRect rt, size_t dataLen = 0, BYTE* pData = NULL);
	void DoModalDialog(CRect rt, const CString& caption, CWnd* parent = NULL, const CString& fontname = "Arial", short fontsize = 9, DWORD style = WS_CAPTION | WS_VISIBLE | WS_DLGFRAME | WS_POPUP | WS_SYSMENU | DS_MODALFRAME | DS_SETFONT , DWORD exStyle = 0);

	CSettingDialog();
	virtual ~CSettingDialog();
};

