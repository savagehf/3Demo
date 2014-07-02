// DlgChart.cpp : implementation file
//

#include "stdafx.h"
#include "3Demo.h"
#include "DlgChart.h"
#include "MainFrm.h"
#include "chartctrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartXYSerie.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartPointsSerie.h"
#include "chartctrl/ChartStandardAxis.h"


IMPLEMENT_DYNAMIC(CDlgChart, CDialog)

CDlgChart::CDlgChart(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChart::IDD, pParent)
{
	m_nFirstSerieID = 0;
	m_nSecSerieID   = 0;
	m_dXValue = 0;
	m_dYValue = 0;
}

CDlgChart::~CDlgChart()
{
}

void CDlgChart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARTCTRL, m_ChartCtrlFisrt);
	DDX_Control(pDX, IDC_CHARTCTRL2, m_ChartCtrlSecond);
	DDX_Control(pDX, IDC_START_EXPLORE_1, m_colorBtnRoute1);
	DDX_Control(pDX, IDC_START_EXPLORE_2, m_colorBtnRoute2);
	DDX_Control(pDX, IDC_BTN_GET_POSITION, m_colorBtnCalc);
}


BEGIN_MESSAGE_MAP(CDlgChart, CDialog)
	//ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START_EXPLORE_1, &CDlgChart::OnBnClickedStartExplore1)
	ON_BN_CLICKED(IDC_START_EXPLORE_2, &CDlgChart::OnBnClickedStartExplore2)
	ON_BN_CLICKED(IDC_BTN_GET_POSITION, &CDlgChart::OnBnClickedBtnGetPosition)
	ON_BN_CLICKED(IDC_BTN_MORE_POS, &CDlgChart::OnBnClickedBtnMorePos)
END_MESSAGE_MAP()


int CDlgChart::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES;


	return 0;
}

BOOL CDlgChart::OnInitDialog()
{

	CDialog::OnInitDialog();

	m_colorBtnRoute1.SetFlat(FALSE);
 	m_colorBtnRoute1.SetTooltipText(_T("test"));
 	m_colorBtnRoute1.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
 	m_colorBtnRoute1.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255,255,255));
 	m_colorBtnRoute1.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
 	m_colorBtnRoute1.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255,255,255));
 	//m_colorBtnRoute1.DrawBorder();

	m_colorBtnRoute2.SetFlat(FALSE);
	m_colorBtnRoute2.SetTooltipText(_T("test"));
	m_colorBtnRoute2.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
	m_colorBtnRoute2.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255,255,255));
	m_colorBtnRoute2.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
	m_colorBtnRoute2.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255,255,255));
	m_colorBtnRoute2.DrawBorder();

	m_colorBtnCalc.SetFlat(FALSE);
	m_colorBtnCalc.SetTooltipText(_T("test"));
	m_colorBtnCalc.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
	m_colorBtnCalc.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255,255,255));
	m_colorBtnCalc.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
	m_colorBtnCalc.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255,255,255));
	m_colorBtnCalc.DrawBorder();




	//Create first chartctrl
	CChartStandardAxis* pBottomAxis = 
		m_ChartCtrlFisrt.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(0, 1600);
	CChartStandardAxis* pLeftAxis =
		m_ChartCtrlFisrt.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(0, 120);

	CChartXYSerie* pSeries = NULL;
	CChartPointsSerie* pPointSeries = m_ChartCtrlFisrt.CreatePointsSerie(false, false);
	pSeries = pPointSeries;
	pSeries->SetName(_T("采样数据"));
	m_nFirstSerieID = pPointSeries->GetSerieId();

	//set title.
	CChartTitle* pTitle = m_ChartCtrlFisrt.GetTitle();
	pTitle->RemoveAll();
	pTitle->AddString(_T("采样脏弹数据"));
	
	//Create Second ChartCtrl;
	CChartStandardAxis* pSecBottomAxis = 
		m_ChartCtrlSecond.CreateStandardAxis(CChartCtrl::BottomAxis);
	pSecBottomAxis->SetMinMax(0, 1600);
	CChartStandardAxis* pSecLeftAxis =
		m_ChartCtrlSecond.CreateStandardAxis(CChartCtrl::LeftAxis);
	pSecLeftAxis->SetMinMax(0, 120);
	CChartXYSerie* pSecSeries = NULL;
	CChartLineSerie* pLineSeries = m_ChartCtrlSecond.CreateLineSerie(false,false);
	pSecSeries = pLineSeries;
	pSecSeries->SetName(_T("采样数据"));
	pLineSeries->SetColor(RGB(0,0,255));
	pLineSeries->SetWidth(3);
	m_nSecSerieID = pLineSeries->GetSerieId();
	CChartTitle* pSecTitle = m_ChartCtrlSecond.GetTitle();
	pSecTitle->RemoveAll();
	pSecTitle->AddString(_T("采样脏弹数据"));
	int NumberPoints = 10;
	/*double* XValues = new double[NumberPoints];
	double* YValues = new double[NumberPoints];
	float Step = (100 - 0)/(10);
	float XStart = 0;
	float Slope = 10;
	float Offset = 50;
	for (int i=0;i<NumberPoints;i++)
	{
		XValues[i] =i * Step;
		YValues[i] = XValues[i] * XValues[i]/Slope + Offset;
	}
	pSecSeries->SetPoints(XValues,YValues,NumberPoints);

	delete[] XValues;
	delete[] YValues;*/

	//m_ChartCtrlFisrt.RefreshCtrl();
	//m_ChartCtrlSecond.RefreshCtrl();

	return TRUE;
}

#define STEP_X 50
#define STEP_Y 70
//void CDlgChart::OnTimer(UINT_PTR nIDEvent)
//{
//	//if (nIDEvent == TIMER_ADD_DATA)
//	{
//		CChartPointsSerie* pPointSeries = (CChartPointsSerie*)m_ChartCtrlFisrt.GetSerie(m_nFirstSerieID);
//		CChartLineSerie* pLineSeries = (CChartLineSerie*)m_ChartCtrlSecond.GetSerie(m_nSecSerieID);
//		if (NULL != pPointSeries && NULL != pLineSeries)
//		{
//			if (m_dXValue>=500)
//			{
//				m_dXValue += STEP_X;
//				m_dYValue -= STEP_Y;
//			}
//			else
//			{
//				m_dXValue += STEP_X;
//				m_dYValue += STEP_Y;
//			}
//
//			pPointSeries->AddPoint(m_dXValue, m_dYValue);
//			pLineSeries->AddPoint(m_dXValue, m_dYValue);
//			m_ChartCtrlFisrt.RefreshCtrl();
//			m_ChartCtrlSecond.RefreshCtrl();
//		}
//	}
//}
//

void CDlgChart::OnBnClickedStartExplore1()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != NULL)
	{
		pFrame->PostMessage(WM_START_TASK_1);
	}
}

void CDlgChart::OnBnClickedStartExplore2()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->PostMessage(WM_START_TASK_2);
	}
}

void CDlgChart::OnBnClickedBtnGetPosition()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->PostMessage(WM_CALC_POSITION);
	}
}

//将fpos进行缩放到在X轴范围内。
//magic number;8.0.这个值是在3D场景中，飞行的路径的长度值的/2 = 8.0，
//如果3D中的坐标更改了，此处也需要更改哦。
//函数方程：y = -(x-8.0)*(x-8.0) + 100;
void CDlgChart::AddData1(float fPos, float fDesity)
{
	float fXPos = fPos*100;
	float fYDes = -(fPos-8.0)*(fPos-8.0) + 100;
	CChartPointsSerie* pPointSeries = (CChartPointsSerie*)m_ChartCtrlFisrt.GetSerie(m_nFirstSerieID);
	
	if (NULL != pPointSeries)
	{
		pPointSeries->AddPoint(fXPos, fYDes);	
		m_ChartCtrlFisrt.RefreshCtrl();
	}
	
}

void CDlgChart::AddData2(float fPos, float fDesity)
{
	float fXPos = fPos*100;
	float fYDes = -(fPos-8.0)*(fPos-8.0) + 100;
	CChartLineSerie* pLineSeries = (CChartLineSerie*)m_ChartCtrlSecond.GetSerie(m_nSecSerieID);

	if (NULL != pLineSeries)
	{
		pLineSeries->SetWidth(2.0);
		pLineSeries->AddPoint(fXPos, fYDes);
		m_ChartCtrlSecond.RefreshCtrl();
	}
}
void CDlgChart::OnBnClickedBtnMorePos()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL != pFrame)
	{
		pFrame->PostMessage(WM_MORE_POSITION);
	}
}
