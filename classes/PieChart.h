#pragma once

#include <afxwin.h>
#include <AFXTEMPL.H>

class PieChart;
struct Pizza
{
	Pizza(CString name = "pizza", double value = 1, COLORREF cr = RGB(255,0,0)):name(name), value(value), color(cr), large(FALSE){}
	BOOL CheckPoint(CPoint pt);
	void DrawPie(CDC&);
	void DrawLabel(CDC&);
	void SetPie(CRect pieRt, double aStart, double aEnd);

	CString name;
	double value;
	COLORREF color;
	double ratio;
	BOOL large;

friend PieChart;
private:
	void _SetPie(CRect pieRt, double as, double ae, CPoint& pt1, CPoint& pt2, CPoint& txPt );
	CPoint pt1, pt2, txPt;
	CPoint pt1m, pt2m, txPtm;
	CRect pieRect, largePieRect;
};

typedef CMap<CString, LPCSTR, COLORREF, COLORREF&> PIECOLOR;

class PieChart  
{
public:
	PieChart();
	virtual ~PieChart();

	void SetRect(const CRect& rt);
	void Draw(CDC&);
	void SetFont(CFont* font){ m_pFont = font; }
	void SetBorderPen(CPen* pen){ m_pBorderPen = pen; }

	void SetPizza(Pizza& p){ m_pizza[p.name] = p; _resetPoints(); }

	void SetData(CString name, double value, COLORREF c){ Pizza p(name, value, c); SetPizza(p); }

	CString PointWhichPie(CPoint pt);
	void EnlargePie(CString name);
	void GetAllPieColor(PIECOLOR& map);

private:
	void SaveContext(CDC&);
	void RestoreContext(CDC&);

	CBrush* m_pOldBr;
	CFont* m_pOrgFont;
	CPen* m_pOrgPen;
	int m_bkMode;
	COLORREF m_oldTxCr;

private:
	void _resetPoints();

	COLORREF m_bgColor;
	CRect m_rt, m_pieRt;
	CPoint m_ct;

	CFont *m_pFont;
	CPen *m_pBorderPen;

	CArray<COLORREF, COLORREF> m_pieColor;
	CMap<CString, LPCSTR, Pizza, Pizza&> m_pizza;

	int	m_radius;
};
