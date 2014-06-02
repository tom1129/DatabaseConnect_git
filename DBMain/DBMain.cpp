#include "stdafx.h"
#include "stdlib.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>


//#pragma  comment(lib, "DBNode.lib")   

using namespace std;

typedef int(*lpDBGetFun)(int**,int*,int*,int*,double*,double*,int*,int*,double*,double*,double*,int*,double*); //�궨�庯��ָ������

//some predefined number
const int Mhl_info_Row = 86; //��¼�ϵ���Ϣ�ı���tbl_Input_Pipe_Process_Elevation������
const int Mhl_info_Column = 6;  //��¼�ϵ���Ϣ�ı���tbl_Input_Pipe_Process_Elevation��������Ϣ������
const int NodeNum = 97; //�����Ľڵ����
const int PipeNum = 96; //�������ĸ���

int main( int argc, char *argv[] )
{
	HINSTANCE hDll; //DLL ���
	lpDBGetFun DBGetFun; //����ָ��
	hDll = LoadLibrary(_T("ODBCconnect.dll"));

	// some variables from the original program
	int arcRow = PipeNum;
	int arcCol = 3;
	int **parc = new int*[arcRow];
	for (int arci = 0;arci<arcRow;arci++)
		parc[arci] = new int[arcCol];
	int **inarc = parc;
	int indiameter[PipeNum];
	int inlength[PipeNum];
	int inheight[NodeNum];
	double ingas_pres_Lb[NodeNum];
	double ingas_pres_Ub[NodeNum];
	int inpara_T_Lb[NodeNum];
	int inpara_T_Ub[NodeNum];
	double incost[NodeNum];
	double ingas_flow_Lb[NodeNum];
	double ingas_flow_Ub[NodeNum];
	int inpara_T0[PipeNum];
	double inpara_Mhl[PipeNum];

	if (hDll != NULL)
	{
		DBGetFun = (lpDBGetFun)GetProcAddress(hDll, "sqliteconnect");
		if (DBGetFun != NULL)
		{
			int result = DBGetFun(inarc,indiameter,inlength,inheight,ingas_pres_Lb,ingas_pres_Ub,inpara_T_Lb,inpara_T_Ub,incost,ingas_flow_Lb,ingas_flow_Ub,inpara_T0,inpara_Mhl);
			printf ("%d", result);
		}
		else
			cout<<"DBGetFun �Ҳ�����"<<endl;
		FreeLibrary(hDll);
	}
	else{
		cout<<"hDll �Ҳ�����"<<endl;
	}

	//hDll_2 = LoadLibrary(_T("DBNode.dll")); //ODBC�޷���̬�����࣬���������ֻ�ܾ�̬����

	//DBNode m_dbnode;

	system("pause");
	return 0;
}