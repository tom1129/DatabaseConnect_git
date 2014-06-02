// DB_CON.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<afxdb.h>
#include "ODBCconnect.h"
#include"Resource.h"
#include "lib.h"

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
#include <iostream>
#include <fstream>
using namespace std;

	const int Mhl_info_Row = 86; //记录断点信息的表（如tbl_Input_Pipe_Process_Elevation）行数
	const int Mhl_info_Column = 6;  //记录断点信息的表（如tbl_Input_Pipe_Process_Elevation）所需信息的列数
	const int NodeNum = 97; //管网的节点个数
	const int PipeNum = 96; //管网弧的个数
	const int MaxNum = 10000;

	enum data_type{type_double, type_int, type_bool};
	//open the Record set and check if it is opened properly
	bool open_check(CRecordset& rs, CString strSQL){
		try{
			rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
		}
		catch(CDBException* pe)
		{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return true;
		}
		catch(CMemoryException* me)
		{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return true;
		}
		return false;

	}

	//read the item 

	//For the data of double
	bool read_item(int item_num, CRecordset &rs, double*item_set, char* filedValue, int step_size = sizeof(double*))
	{
		CString strDbVar;
		rs.MoveFirst( );
		int counter=0;
		while(counter <item_num ) 	
		{ 
			//intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
			rs.GetFieldValue(_T(filedValue),strDbVar); 
			*(item_set+counter * step_size) = atof(strDbVar);
			counter=counter+1;
			rs.MoveNext();
		}
	}
	//For the data of int
	bool read_item(int item_num, CRecordset &rs, int*item_set, char* filedValue,bool is_ID = false, int step_size = sizeof(int*))
	{
		CString strDbVar;
		rs.MoveFirst( );
		int counter=0;
		while(counter <item_num ) 	
		{ 
			//intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
			rs.GetFieldValue(_T(filedValue),strDbVar); 
			*(item_set+counter * step_size) = atoi(strDbVar);
			if(is_ID)
				*(item_set+counter * step_size) = *(item_set+counter * step_size) - 1; // the input item is ID, then it should start from 0 instead of 1
			counter=counter+1;
			rs.MoveNext();
		}
	}
	//For the data of bool
	bool read_item(int item_num, CRecordset &rs, bool*item_set, char* filedValue,bool is_ID = false, int step_size = sizeof(int*))
	{
		CString strDbVar;
		rs.MoveFirst( );
		int counter=0;
		while(counter <item_num ) 	
		{ 
			//intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
			rs.GetFieldValue(_T(filedValue),strDbVar); 
			*(item_set+counter * step_size) = (bool)atoi(strDbVar);
			counter=counter+1;
			rs.MoveNext();
		}
	}
	// For the undefined type
	template <class T>
	bool read_item(int item_num, CRecordset &rs, T*item_set, char* filedValue, enum data_type type, int step_size)
	{
		CString strDbVar;
		rs.MoveFirst( );
		int counter=0;
		while(intRow <item_num ) 	
		{ 
			//intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
			rs.GetFieldValue(_T(filedValue),strDbVar); 
			switch(type){
			case type_double:
				*(item_set+intRow * step_size) = atoi(strDbVar);
				break;
			case type_int:
				*(item_set+intRow * step_size) = atof(strDbVar);
				break;
			case type_bool:
				*(item_set+intRow * step_size) = atoi(strDbVar);
				break;
			default:
				cout<<"Haven't define this type!"<<endl;
				return false;
				break;
			}

			intRow=intRow+1;

			rs.MoveNext();
		}
	}


	int get_arc(int **arc, int arcRow, CDatabase& db, CRecordset& rs) // ??????????????????? what's the function for
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Pipe_Static";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		if(open_check(rs, strSQL))
			return 0;
	

		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			return 0;
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <arcRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("UpNodeID"),strDbVar); 
					arc[intI][0] = atoi(strDbVar)-1;
					intRow=intRow+1;

					rs.MoveNext();
			}

		  rs.MoveFirst( );
			intRow=0;
          while(intRow <arcRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("DownNodeID"),strDbVar); 
					arc[intI][1] = atoi(strDbVar)-1;
					intRow=intRow+1;

					rs.MoveNext();
			}

		  rs.MoveFirst( );
			intRow=0;
          while(intRow <arcRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("PipeTypeID"),strDbVar); 
					arc[intI][2] = atoi(strDbVar)-1;
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	 
	}

	int get_diameter(int *diameter,int diameterRow, CDatabase& db, CRecordset& rs)
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Pipe_Process_Fixed";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;
		cout << "testing! "<<endl;
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
         while(intRow <diameterRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("InsideDiameter"),strDbVar); 
					diameter[intI] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_length(int *length,int lengthRow,CDatabase& db, CRecordset& rs)
	{
	    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Pipe_Process_Fixed";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <lengthRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("Length"),strDbVar); 
					length[intI] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_height(int *height,int heightRow, CDatabase& db, CRecordset& rs)
	{
     	    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Node_Process_Fixed";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <heightRow ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("Elevation"),strDbVar); 
					height[intI] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_gas_pres_Lb(double *gas_pres_Lb,int gas_pres_Lb_Row, CDatabase& db, CRecordset& rs)
	{
		    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Node_Process_Alterable_Year";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <gas_pres_Lb_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MinPressure"),strDbVar); 
				    gas_pres_Lb[intI] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_gas_pres_Ub(double *gas_pres_Ub,int gas_pres_Ub_Row, CDatabase& db, CRecordset& rs)
	{
		    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Node_Process_Alterable_Year";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <gas_pres_Ub_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MaxPressure"),strDbVar); 
					gas_pres_Ub[intI] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_para_T_Lb(int *para_T_Lb,int para_T_Lb_Row, CDatabase& db, CRecordset& rs)
	{
		    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Node_Process_Alterable_Year";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <para_T_Lb_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MinTemperature"),strDbVar); 
					para_T_Lb[intI] = atoi(strDbVar)+273;
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_para_T_Ub(int *para_T_Ub,int para_T_Ub_Row, CDatabase& db, CRecordset& rs)
	{
		    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Node_Process_Alterable_Year";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;

		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <para_T_Ub_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MaxTemperature"),strDbVar); 
					para_T_Ub[intI] = atoi(strDbVar)+273;
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}
	
	int get_cost(double *cost,int costRow, CDatabase& db, CRecordset& rs)
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	  
		int intI=0;
		int costID[NodeNum];
		int costLength = 0;

		//赋初值
	  for(intI = 0;intI<costRow; intI++)
	  {
	  cost[intI] = 0.0;
	  }

	  //得到Client部分的cost
	strTableName = "tbl_Input_Client_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			costLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=costLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					costID[costLength] = atoi(strDbVar)-1;
					costLength = costLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Client_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <costLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("UnitAlterablePrice"),strDbVar); 
					cost[costID[intRow]] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();



		//得到Source部分的cost
	strTableName = "tbl_Input_Source_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			costLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=costLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					costID[costLength] = atoi(strDbVar)-1;
					costLength = costLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Source_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <costLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("UnitAlterableCost"),strDbVar); 
					cost[costID[intRow]] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();


		return 1;
	}

	int get_gas_flow_Lb(double *gas_flow_Lb,int flowRow, CDatabase& db, CRecordset& rs)
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	  
		int intI=0;
		int flowID[NodeNum];
		int flowLength = 0;

		//赋初值
	  for(intI = 0;intI<flowRow; intI++)
	  {
	  gas_flow_Lb[intI] = 0.0;
	  }

	  //得到Client部分的gas_flow_Lb
	strTableName = "tbl_Input_Client_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			flowLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=flowLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					flowID[flowLength] = atoi(strDbVar)-1;
					flowLength = flowLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Client_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <flowLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MaxFlowRate"),strDbVar); 
					gas_flow_Lb[flowID[intRow]] = -atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();



		//得到Source部分的gas_flow_Lb
	strTableName = "tbl_Input_Source_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			flowLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=flowLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					flowID[flowLength] = atoi(strDbVar)-1;
					flowLength = flowLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Source_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <flowLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MinFlowRate"),strDbVar); 
					gas_flow_Lb[flowID[intRow]] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();


		return 1;
	}

	int get_gas_flow_Ub(double *gas_flow_Ub,int flowRow, CDatabase& db, CRecordset& rs)
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	  
		int intI=0;
		int flowID[NodeNum];
		int flowLength = 0;

		//赋初值
	  for(intI = 0;intI<flowRow; intI++)
	  {
	  gas_flow_Ub[intI] = 0.0;
	  }

	  //得到Client部分的gas_flow_Ub
	strTableName = "tbl_Input_Client_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			flowLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=flowLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					flowID[flowLength] = atoi(strDbVar)-1;
					flowLength = flowLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Client_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <flowLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MinFlowRate"),strDbVar); 
					gas_flow_Ub[flowID[intRow]] = -atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();



		//得到Source部分的gas_flow_Ub
	strTableName = "tbl_Input_Source_Static";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			flowLength = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=flowLength;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("NodeID"),strDbVar);
					flowID[flowLength] = atoi(strDbVar)-1;
					flowLength = flowLength+1;

					rs.MoveNext();
			}

		}
		rs.Close();

		strTableName = "tbl_Input_Source_Dynamic_Month_Month";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <flowLength ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("MaxFlowRate"),strDbVar); 
					gas_flow_Ub[flowID[intRow]] = atof(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();


		return 1;
	}

	int get_para_T0(int *para_T0,int para_T0_Row, CDatabase& db, CRecordset& rs)
	{
		CString strTableName;
		CString strSQL;
		CString strDbVar;
	  
		int intI=0;
		int T_ID[MaxNum];
		int T_Length = 0;

		//赋初值
	  for(intI = 0;intI<para_T0_Row; intI++)
	  {
	  para_T0[intI] = 0;
	  }

	strTableName = "tbl_Input_Pipe_Process_Alterable_Year";
	strSQL.Format(_T("select * From %s "),strTableName);
		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			}
		else
		{
			rs.MoveFirst( );
			T_Length = 0;
          while(!rs.IsEOF()) 	
				{ 
					intI=T_Length;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("PipeID"),strDbVar);
					T_ID[T_Length] = atoi(strDbVar)-1;
					T_Length = T_Length+1;

					rs.MoveNext();
			}

		  rs.MoveFirst( );
			int intRow=0;
          while(intRow <T_Length ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("AmbientTemperature"),strDbVar); 
					para_T0[T_ID[intRow]] = atoi(strDbVar)+273;
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_temp(int **temp,CDatabase& db, CRecordset& rs)
	{
	    CString strTableName;
		CString strSQL;
		CString strDbVar;
	    strTableName = "tbl_Input_Pipe_Process_Elevation";
		strSQL.Format(_T("select * From %s "),strTableName);
		
		int intI=0;
		int i,j;

		//初始化
	  for (i = 0;i<Mhl_info_Row;i++)
	  {
	  for (j = 0;j<Mhl_info_Column;j++)
	  {
	   temp[i][j] = 0;
	  }
	  }


		
	try{
		rs.Open(AFX_DB_USE_DEFAULT_TYPE,strSQL); 
	}
	catch(CDBException* pe)
	{
			cout<<"错误代码:"<<pe->m_nRetCode<<endl;
			cout<<"错误信息:"<<pe->m_strError<<endl;
			pe->Delete();
			system("pause");
			return 0;
	}
	catch(CMemoryException* me)
	{
			cout<<"内存不足！"<<endl;
			me->Delete();	
			system("pause");
			return 0;
	}


		if(rs.IsBOF())
		{
			rs.Close();
			db.Close();
			return 0;
			}
		else
		{
			rs.MoveFirst( );
			int intRow=0;
          while(intRow <Mhl_info_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("PipeID"),strDbVar); 
					temp[intI][0] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		  rs.MoveFirst( );
			intRow=0;
          while(intRow <Mhl_info_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("PointCount"),strDbVar); 
					temp[intI][1] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		  rs.MoveFirst( );
			intRow=0;
          while(intRow <Mhl_info_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("Length"),strDbVar); 
					temp[intI][2] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		   rs.MoveFirst( );
			intRow=0;
          while(intRow <Mhl_info_Row ) 	
				{ 
					intI=intRow;//由于是从小到大读取,因此可以直接按当前行读取
					rs.GetFieldValue(_T("Elevation"),strDbVar); 
					temp[intI][3] = atoi(strDbVar);
					intRow=intRow+1;

					rs.MoveNext();
			}

		}
		rs.Close();
		return 1;
	}

	int get_para_Mhl(double *para_Mhl,int para_Mhl_Row, int *length, int *height, CDatabase& db, CRecordset& rs)
	{
	//初始化
		for(int iMhl=0;iMhl<para_Mhl_Row;iMhl++)
		{
		para_Mhl[iMhl] = 0.0;
		}

		//得到temp的值
	   // int Mhl_info_Row = 86;
	   //int Mhl_info_Column = 6;
		int **ptemp = new int*[Mhl_info_Row];
		for (int tempi = 0;tempi<Mhl_info_Row;tempi++)
			ptemp[tempi] = new int[Mhl_info_Column];
		int **temp = ptemp;
        get_temp(temp,db,rs); //checked

		//计算Mhl
		int i=1;
		int j,arc,start_p,end_p,poi,seg;
		int h[100],l[100];

		while(i<=Mhl_info_Row)
		{
    arc = temp[i-1][0];
    start_p = arc;
    end_p = arc+1;
    poi = temp[i-1][1];
    seg = poi+1;

    h[0] = height[start_p-1];
    for(j=1;j<=poi;j++)
	{
        h[j]=temp[i+j-2][3];
	}
    h[seg]=height[end_p-1];

    l[0]=temp[i-1][2];
    for (j=2;j<=poi;j++)
	{
        l[j-1]=temp[i+j-2][2]-temp[i+j-3][2];
	}
    l[seg-1] = length[arc-1]-temp[i+poi-2][2];
    
    for(j=1;j<=(seg+1);j++)
	{
        h[seg+2-j-1]=h[seg+2-j-1]-h[0];
	}
    
    para_Mhl[arc-1]=0;
    for (j=1;j<=seg;j++)
	{
        para_Mhl[arc-1] = para_Mhl[arc-1]+0.5*l[j-1]*(h[j-1]+h[j]);
	}
	
    i=i+poi;
		}


		return 0;
	}



//int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
	 int sqliteconnect(int **arc,int *diameter,int *length,int *height,double *gas_pres_Lb,double *gas_pres_Ub,int *para_T_Lb,int *para_T_Ub,double *cost,double *gas_flow_Lb,double *gas_flow_Ub,int *para_T0,double *para_Mhl)
{
	int diameterRow,arcRow,lengthRow,heightRow,gas_pres_Row,para_T_Row,costRow,gas_flow_Row,para_T0_Row,para_Mhl_Row;
	//维度赋值——————————————————————
	arcRow = PipeNum;  diameterRow = PipeNum; lengthRow = PipeNum; para_T0_Row = PipeNum; para_Mhl_Row = PipeNum; 
	heightRow = NodeNum;  gas_pres_Row = NodeNum;  para_T_Row = NodeNum; costRow = NodeNum;  gas_flow_Row = NodeNum;
	
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CDatabase db;
		CString strOpenDB;
		CString msg;
		CString strMessage;

		
		CString DBadress="E:\\document\\assignment\\DatebashInterface\\gas_data.db";

		strOpenDB="Driver={SQLite3 ODBC Driver};Database="+DBadress+";StepAPI=0;SyncPragma=NORMAL;NoTXN=0;Timeout=100000;ShortNames=0;LongNames=0;NoCreat=0;NoWCHAR=1;FKSupport=0;JournalMode=;OEMCP=0;LoadExt=;BigInt=0;JDConv=0;";
	
		
	try{
		db.OpenEx(strOpenDB, CDatabase::noOdbcDialog);
		cout<<"Open Success!"<<endl;
		
	}
		catch(CDBException* pe)
	{
		cout<<"错误代码:"<<pe->m_nRetCode<<endl;
		cout<<"错误信息:"<<pe->m_strError<<endl;
		pe->Delete();
		system("pause");
		return 0;
	}

		CRecordset rs(&db);
		//以下函数(return 0)报错；(return 1)运行正常
	    get_diameter(diameter,diameterRow,db,rs); //checked
		get_arc(arc,arcRow,db,rs); //checked
		get_cost(cost,costRow,db,rs); //checked
		get_length(length,lengthRow,db,rs); //checked
		get_height(height,heightRow,db,rs);  //checked
		get_gas_pres_Lb(gas_pres_Lb,gas_pres_Row,db,rs); //checked
		get_gas_pres_Ub(gas_pres_Ub,gas_pres_Row,db,rs); //checked
		get_para_T_Lb(para_T_Lb,para_T_Row,db,rs); //checked
		get_para_T_Ub(para_T_Ub,para_T_Row,db,rs); //checked
		get_gas_flow_Lb(gas_flow_Lb,gas_flow_Row,db,rs); //checked
		get_gas_flow_Ub(gas_flow_Ub,gas_flow_Row,db,rs); //checked
		get_para_T0(para_T0,para_T0_Row,db,rs);  //checked
		get_para_Mhl(para_Mhl,para_Mhl_Row,length,height,db,rs); //checked
		
		db.Close();
		//std::cout<<"close source."<<endl;
		std::cout<<"read end"<<endl;

	}
	return nRetCode;
}


