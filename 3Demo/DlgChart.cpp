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

#define TIMER_ADD_DATA 2100


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
}


BEGIN_MESSAGE_MAP(CDlgChart, CDialog)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START_EXPLORE_1, &CDlgChart::OnBnClickedStartExplore1)
	ON_BN_CLICKED(IDC_START_EXPLORE_2, &CDlgChart::OnBnClickedStartExplore2)
	ON_BN_CLICKED(IDC_BTN_GET_POSITION, &CDlgChart::OnBnClickedBtnGetPosition)
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

	//Create first chartctrl
	CChartStandardAxis* pBottomAxis = 
		m_ChartCtrlFisrt.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(0, 1000);
	CChartStandardAxis* pLeftAxis =
		m_ChartCtrlFisrt.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(0, 1000);

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
	pSecBottomAxis->SetMinMax(0, 1000);
	CChartStandardAxis* pSecLeftAxis =
		m_ChartCtrlSecond.CreateStandardAxis(CChartCtrl::LeftAxis);
	pSecLeftAxis->SetMinMax(0, 1000);
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

	SetTimer(TIMER_ADD_DATA, 3000, NULL);

	return TRUE;
}

#define STEP_X 50
#define STEP_Y 70
void CDlgChart::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ADD_DATA)
	{
		CChartPointsSerie* pPointSeries = (CChartPointsSerie*)m_ChartCtrlFisrt.GetSerie(m_nFirstSerieID);
		CChartLineSerie* pLineSeries = (CChartLineSerie*)m_ChartCtrlSecond.GetSerie(m_nSecSerieID);
		if (NULL != pPointSeries && NULL != pLineSeries)
		{
			if (m_dXValue>=500)
			{
				m_dXValue += STEP_X;
				m_dYValue -= STEP_Y;
				//int nYRand = rand()%100;
				//m_dYValue -=nYRand;
			}
			else
			{
				m_dXValue += STEP_X;
				m_dYValue += STEP_Y;
// 				int nYRand = rand()%100;
// 				m_dYValue += nYRand;
			}

			pPointSeries->AddPoint(m_dXValue, m_dYValue);
			pLineSeries->AddPoint(m_dXValue, m_dYValue);
			m_ChartCtrlFisrt.RefreshCtrl();
			m_ChartCtrlSecond.RefreshCtrl();
		}
	}
}


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
