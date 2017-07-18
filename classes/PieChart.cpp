#include "PieChart.h"
#include <math.h>

#define M_PI acos(-1.0)

#define LIGHT_COLOR RGB(210, 210, 210)
#define DARK_COLOR RGB(10, 10, 10)

PieChart::PieChart()
:m_bgColor(RGB(255,0,0))
,m_radius(1)
,m_pFont(NULL)
,m_pBorderPen(NULL)
{
	m_pieColor.Add(RGB(255,0,0));
	m_pieColor.Add(RGB(0,255,0));
	m_pieColor.Add(RGB(0,0,255));
/*
 	m_pizza["1011"] = (Pizza("1011", 1, RGB(255,1,10)));
 	m_pizza["1012"] = (Pizza("1012", 1.5, RGB(255,255,1)));
 	m_pizza["1013"] = (Pizza("1013", 0.5, RGB(255,1,1)));
 	m_pizza["2011"] = (Pizza("2011", 3, RGB(50,1,255)));
 	m_pizza["3011"] = (Pizza("3011", 2.1, RGB(255,50,1)));
 	m_pizza["4011"] = (Pizza("4011", 2, RGB(50,1,1)));
*/
 }

PieChart::~PieChart()
{
	
}

#define MARGIN 20
void PieChart::SetRect(const CRect &rt)
{
	m_rt = CRect( rt.left + MARGIN, rt.top + MARGIN, rt.right - MARGIN, rt.bottom - MARGIN);

	m_ct = m_rt.CenterPoint();
	BOOL isWide = m_rt.Width() > m_rt.Height();
	m_radius = isWide? m_rt.Height()/2: m_rt.Width()/2;

	_resetPoints();
}

void PieChart::_resetPoints()
{
	POSITION pos = m_pizza.GetStartPosition();
	double sum = 0, as = 0, ae = 0, txtPos = 0;
	CString key;
	Pizza pizza;
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, pizza);
		sum += pizza.value;
	}

	pos = m_pizza.GetStartPosition();
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, pizza);
		Pizza& p = m_pizza[key];

		p.ratio = pizza.value / sum;
		as = ae;
		ae += p.ratio * 2 * M_PI;

		CRect pieRt = CRect( m_ct.x - m_radius, m_ct.y - m_radius, m_ct.x + m_radius, m_ct.y + m_radius );
		p.SetPie( pieRt, as, ae);
	}
}

BOOL _isDark(COLORREF cr)
{
	int totalColor = ((int)(BYTE)cr) + ((int)(BYTE)(cr>>8)) + ((int)(BYTE)(cr>>16));
	return totalColor < (255);
}

void PieChart::SaveContext(CDC& dc)
{
	CBrush brush(m_bgColor);
	m_pOldBr = dc.SelectObject(&brush);
	m_pOrgFont = dc.SelectObject(m_pFont);
	m_pOrgPen = dc.SelectObject(m_pBorderPen);
	m_bkMode = dc.SetBkMode(TRANSPARENT);
	m_oldTxCr = dc.SetTextColor(DARK_COLOR);
}

void PieChart::RestoreContext(CDC& dc)
{
	dc.SetTextColor(m_oldTxCr);
	dc.SetBkMode(m_bkMode);
	dc.SelectObject(m_pOldBr);
	dc.SelectObject(m_pOrgFont);
	dc.SelectObject(m_pOrgPen);
}

void PieChart::Draw(CDC& dc)
{
	SaveContext(dc);

	POSITION pos = m_pizza.GetStartPosition();
	CString key, label;
	Pizza p;
	//draw pie
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, p);
		p.DrawPie(dc);
	}
	//draw text
	pos = m_pizza.GetStartPosition();
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, p);
		p.DrawLabel(dc);
	}

	RestoreContext(dc);
}

CString PieChart::PointWhichPie(CPoint pt)
{
	CString key, focusKey; Pizza p;
	POSITION pos = m_pizza.GetStartPosition();
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, p);
		m_pizza[key].large = p.CheckPoint(pt);
		if (m_pizza[key].large)
			focusKey = key;
	}

	return focusKey;
}


void PieChart::GetAllPieColor(PIECOLOR& map)
{
	map.RemoveAll();
	CStringArray arr;
	CString key; Pizza p;
	POSITION pos = m_pizza.GetStartPosition();
	while(pos)
	{
		m_pizza.GetNextAssoc(pos, key, p);
		map[key] = p.color;
	}
}

/********************/
/*	Pizza struct	*/
/********************/

double getAngle(CSize v)
{
	if (!v.cx) return (v.cy>0)? M_PI/2: - M_PI/2;
	
	/* dimension 2,3 : x<0, 4: x>0, y<0 */
	return atan((double)v.cy/v.cx) + ( (v.cx<0)? M_PI: (v.cy<0)? 2*M_PI: 0 );
}

#define EL 10
void Pizza::SetPie(CRect pieRt, double as, double ae)
{
	pieRect = pieRt;

	_SetPie(pieRect, as, ae, pt1, pt2, txPt);

	//	largePieRect = CRect( pieRt.left - EL, pieRt.top - EL, pieRt.right + EL, pieRt.bottom + EL);
	double mid = (as + ae)/2;
	largePieRect = pieRect;
	largePieRect.OffsetRect( CPoint((int)(EL * cos(mid)), (int)(EL * sin(mid))) );

	_SetPie(largePieRect, as, ae, pt1m, pt2m, txPtm);
}

void Pizza::_SetPie(CRect pieRt, double as, double ae, CPoint& pt1, CPoint& pt2, CPoint& txPt )
{
	CPoint ct = pieRt.CenterPoint();
	int r = pieRt.Width()/2;

	pt1 = CPoint( ct.x + (int)(r * cos(as)), ct.y + (int)(r * sin(as)) );
	pt2 = CPoint( ct.x + (int)(r * cos(ae)), ct.y + (int)(r * sin(ae)) );
	
	double txtPos = (as + ae)/2;
	txPt = CPoint( ct.x + (int)(r*2/3 * cos(txtPos)), ct.y + (int)(r*2/3 * sin(txtPos)) );

}


BOOL Pizza::CheckPoint(CPoint pt)
{
	/* test distance from center */
	int r = pieRect.Width()/2;
	CPoint ct = pieRect.CenterPoint();
	CSize v = (pt - ct);
	double dist = sqrt((double)v.cx*v.cx + (double)v.cy*v.cy);
	
	if (dist > r ) return FALSE;
	
	/* test angle between two vectors */
	CSize v1 = pt1 - ct, v2 = pt2 - ct;	
	
	double a1 = getAngle(v1), a2 = getAngle(v2), a = getAngle(v);
	
	if(v1.cy && !v2.cy && v2.cx) a2 = 2*M_PI;
	
	return (a2 >= a) && (a1 <= a);
}

void Pizza::DrawPie(CDC& dc)
{
	CBrush brush( color );
	dc.SelectObject(&brush);

	if (large)
		dc.Pie(largePieRect, pt2m, pt1m);
	else
		dc.Pie(pieRect, pt2, pt1);
}

void Pizza::DrawLabel(CDC& dc)
{
	if (ratio < 0.005) return;
	CString label;
	
	label.Format("%s:%.0f", name, value);
	dc.SetTextColor( _isDark(color)? LIGHT_COLOR: DARK_COLOR );
	CPoint *p = large? &txPtm: &txPt;
	dc.TextOut(p->x, p->y, label);
	label.Format("%.2f%%", ratio*100);
	dc.TextOut(p->x, p->y + 16, label);
}
