#include "stdafx.h"
#include "stdlib.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>


//#pragma  comment(lib, "DBNode.lib")   

using namespace std;

typedef int(*lpDBGetFun)(int**,int*,int*,int*,double*,double*,int*,int*,double*,double*,double*,int*,double*); //宏定义函数指针类型

//some predefined number
const int Mhl_info_Row = 86; //记录断点信息的表（如tbl_Input_Pipe_Process_Elevation）行数
const int Mhl_info_Column = 6;  //记录断点信息的表（如tbl_Input_Pipe_Process_Elevation）所需信息的列数
const int NodeNum = 97; //管网的节点个数
const int PipeNum = 96; //管网弧的个数

int main( int argc, char *argv[] )
{
	HINSTANCE hDll; //DLL 句柄
	lpDBGetFun DBGetFun; //函数指针
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
			cout<<"DBGetFun 找不到。"<<endl;
		FreeLibrary(hDll);
	}
	else{
		cout<<"hDll 找不到。"<<endl;
	}

	//hDll_2 = LoadLibrary(_T("DBNode.dll")); //ODBC无法动态导出类，会产生错误，只能静态导出

	//DBNode m_dbnode;

	system("pause");
	return 0;
}