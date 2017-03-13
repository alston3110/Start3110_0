#include "../include/common.h"

/* ------------------------------------------------------------------------- */
extern HINSTANCE hMainInst ;
extern HWND      hMainWnd  ;
extern int       iMainRS232StartOrStop ;
extern int       iMainPortNum ;
extern int       ubRS232IsExist ;

/* ------------------------------------------------------------------------- */
extern void vReport_SetInfo( int iItem , const char *cStr ) ;
extern void vRS232_Write( unsigned char *ubData , long ulBytes ) ;
extern void vConsole_Init( void ) ;
extern void vReport_Init( HWND hWnd ) ;
extern BOOL bTimer_Start( HWND mHwnd ) ;
extern void vReport_SetNA( void ) ;
/* ------------------------------------------------------------------------- */
enum __TEST_STATE__ {
    PROC_RS232_START  ,
    PROC_DATA_INIT ,
    PROC_CONNECT_PCBA ,
    PROC_LOGIN ,
    PROC_START_MP ,
    PROC_PRODUCT_NO   ,
    PROC_ETHERNET_MAC ,
    PROC_ETHERNET_MAC1 ,
    PROC_MP_TEST,
    PROC_MP_TEST_SELECT,
    PROC_MP_TEST_TESTING,
    PROC_MP_TEST_RESULT,
    PROC_MP_TEST_OK,
    PROC_MP_TEST_SHOW,
    PROC_EXIT
} ;
#define DEF_TEST_PRODUCT_MAX_LEN        10
#define DEF_TEST_MAC_MAX_LEN            12
#define DEF_TEST_MAC_MAX_ITEM           2
#define DEF_TEST_NUMBER_LEN             2
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
int iTestState = 0 ;
int iTestCount = 0 ;
int iTestSubCount = 0 ;
int iTestTimeOut = 0 ;
int iTestMacIndex = 0 ;
int iTestReturnBool = 0 ;
int iTestResult = 0 ;
int iTestFinish = 0 ;
int iTestEnd = 0 ;
int iTestItem = 0 ;
int iTestShowItem = 0 ;
int iTestSavefile = 0 ;
int iTestProcess = 0 ;
int iTestAuto = 0 ;

char cTestProductNo[16] ;
char cTestMacNum[DEF_TEST_MAC_MAX_ITEM][16] ;
char cTestNum[2];
char cTestReturn[2];
char cTestLoginName[5]="root";
char cTestLoginPassword[16];
char cTestProgName[128] ="/mnt/mmcblk1p1/home/user/MP_TEST/MP_Test";
/* ------------------------------------------------------------------------- */
CHECK_STR stcTestCheckData[] = {
    { (unsigned char *)"Please input Machine Model" , 0 , 0 } ,
    { (unsigned char *)"Input Test Item :" , 0 , 0 } ,
    { (unsigned char *)"Please check" , 0 , 0 } ,
    { (unsigned char *)"<<< Finish" , 0 , 0 } ,
    { (unsigned char *)"Pass  <<<" , 0 , 0 } ,
    { (unsigned char *)"Fail  <<<" , 0 , 0 } ,
    { (unsigned char *)"LED >>>" , 0 , 0 } ,
    { (unsigned char *)"GPIO >>>" , 0 , 0 } ,
    { (unsigned char *)"Audio >>>" , 0 , 0 } ,
    { (unsigned char *)"Lvds >>>" , 0 , 0 } ,
    { (unsigned char *)"3AXIS >>>" , 0 , 0 } ,
    { (unsigned char *)"TouchPanel >>>" , 0 , 0 } ,
    { (unsigned char *)"USB >>>" , 0 , 0 } ,
    { (unsigned char *)"WIFI >>>" , 0 , 0 } ,
    { (unsigned char *)"Ethernet - eth0 >>>" , 0 , 0 } ,
    { (unsigned char *)"Ethernet - eth1 >>>" , 0 , 0 } ,
    { (unsigned char *)"RS232 >>>" , 0 , 0 } ,
    { (unsigned char *)"DDR >>>" , 0 , 0 } ,
    { (unsigned char *)"SDCARD >>>" , 0 , 0 } ,
    { (unsigned char *)">>> Start Test " , 0 , 0 } ,
    { (unsigned char *)">>> Start Auto Run" , 0 , 0 } ,
    { (unsigned char *)"Do you want to test again?" , 0 , 0 } ,
    { (unsigned char *)"Save MP Test File Start." , 0 , 0 } ,
    { (unsigned char *)"===========END==========" , 0 , 0 } ,
    { (unsigned char *)"login:" , 0 , 0 } ,
    { (unsigned char *)"root login on" , 0 , 0 } ,
    { (unsigned char *)"No such file or directory" , 0 , 0 } ,

} ;

/* ------------------------------------------------------------------------- */
void vTest_Check( unsigned char ubCh )
{
    int iLoop ;
    CHECK_STR *pCheck ;

    for ( iLoop = 0 ; iLoop < ( sizeof(stcTestCheckData) / sizeof(CHECK_STR) ) ; iLoop ++ )
    {
        pCheck = (CHECK_STR *)&stcTestCheckData[iLoop] ;
        if ( pCheck->pStr[pCheck->iIndex] == ubCh )
        {
            pCheck->iIndex ++ ;
            if ( pCheck->iIndex >= strlen((const char *)pCheck->pStr) )
            {
                pCheck->iState ++ ;
                pCheck->iIndex = 0 ;
                //MessageBox( NULL , "test" , NULL , NULL ) ;
            }
        }
        else
        {
            pCheck->iIndex = 0 ;
        }

    }
}

void vTest_RS232Start( void )
{
    if ( iTestCount == 0 )
    {

        vStatus_Show( "�г]�w�q�T�� , �ñҰ� UART" ) ;
    }
    if ( iMainPortNum )
    {
        Button_Enable( GetDlgItem( hMainWnd , IDC_BUTTON_START ) , TRUE ) ;
    }
    if ( iMainRS232StartOrStop )
    {
        if ( ubRS232IsExist )
        {
            Button_Enable( GetDlgItem( hMainWnd , IDC_BUTTON_RS232_SCAN ) , FALSE ) ;
            vStatus_Show( "" ) ;
            iTestState = PROC_DATA_INIT ;
            iTestCount = 0 ;
            return ;
        }
    }

    iTestCount ++ ;

    if ( ( iTestCount & 0x7FF) == 0x00 )
    {
        vStatus_Show( "�г]�w�q�T�� , �ñҰ� UART" ) ;
    }
    if ( ( iTestCount & 0x7FF) == 0x400 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_DataInit( void )
{
    int iLoop;
    ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_HIDE);
    ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_HIDE);
    ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_HIDE);
    vConsole_Init( ) ;
    Edit_SetText( GetDlgItem( hMainWnd , IDC_EDIT_CONSOLE ) , "" ) ;

    Button_Enable( GetDlgItem( hMainWnd , IDC_BUTTON_START ) , FALSE ) ;

    Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , FALSE ) ;
    Edit_SetText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , "" ) ;

    Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , FALSE ) ;
    Edit_SetText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , "" ) ;

    Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , FALSE ) ;
    Edit_SetText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "" ) ;

    Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_NO ) , FALSE ) ;
    Edit_SetText( GetDlgItem( hMainWnd , IDC_EDIT_NO ) , "" ) ;

    vReport_SetNA( );

    for ( iLoop = 0 ; iLoop < ( sizeof(stcTestCheckData) / sizeof(CHECK_STR) ) ; iLoop ++ )
    {
        stcTestCheckData[iLoop].iState = 0 ;
    }
    iTestState = PROC_CONNECT_PCBA ;
    iTestCount = 0 ;
}
void vTest_ConnectPCBA( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("PCBA �s�u��") ;
        stcTestCheckData[24].iState = 0;
    }

    iTestCount ++ ;
    if ( stcTestCheckData[24].iState  )
    {
        stcTestCheckData[24].iState = 0;
        iTestState = PROC_LOGIN ;
        iTestCount = 0 ;
        return;
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "PCBA �s�u��" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_login( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("�n�J��..") ;
        cTestLoginName[4] = 0x0d ;
        vRS232_Write((unsigned char *)cTestLoginName,(long)5);
    }

    iTestCount ++ ;
    if ( stcTestCheckData[24].iState  )
    {
        vStatus_Show("�n�J�b�����~!!!") ;
        stcTestCheckData[24].iState = 0;
        iTestState = PROC_EXIT ;
    }else if ( stcTestCheckData[25].iState  )
    {
        vStatus_Show("Start MP TEST") ;
        stcTestCheckData[25].iState = 0;
        iTestState = PROC_START_MP ;
        iTestCount = 0 ;
    }
}

void vTest_StartMp( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("Start MP TEST") ;
        stcTestCheckData[0].iState = 0 ;
        iTestReturnBool = 0 ;
        iTestAuto = 0 ;
        iTestProcess = 0 ;
        cTestProgName[40] = 0x0d ;
        vRS232_Write((unsigned char *)cTestProgName,(long)41);
    }

    iTestCount ++ ;

    if ( stcTestCheckData[0].iState  )
    {
        stcTestCheckData[0].iState = 0;
        iTestState = PROC_PRODUCT_NO ;
        iTestCount = 0 ;
        iTestProcess = 1;
    }else if ( stcTestCheckData[21].iState  )
    {
        stcTestCheckData[21].iState = 0 ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_FAIL ) , "NO" ) ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_OK ) , "YES" ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_SHOW);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_SHOW);
        vStatus_Show("�����x�w�g���� MP TEST, �O�_���s�A����!!!!") ;
        iTestProcess = 2;
    }else if ( stcTestCheckData[1].iState  )
    {
        stcTestCheckData[1].iState = 0 ;
        vStatus_Show("�����x��Serial Number, ���ŦX���`���յ{��, �n�~�򧹦� MP ���ն�?") ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_FAIL ) , "����" ) ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_OK ) , "�~��" ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_SHOW);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_SHOW);
        iTestProcess = 3;
    }else if ( stcTestCheckData[26].iState  )
    {
        vStatus_Show("�䤣��MP���յ{��,�Э��s�]�w!!!!") ;
        stcTestCheckData[26].iState = 0;
        iTestState = PROC_EXIT ;
        iTestCount = 0 ;
    }
    if ( iTestReturnBool )
    {
        if ( iTestProcess == 2 ){
            if(iTestReturnBool==1){
                cTestReturn[0]= 'n' ;
                iTestState = PROC_EXIT;
                vStatus_Show("�Ч󴫤U�@��PCBA") ;
            }
            else if(iTestReturnBool==2){
                cTestReturn[0]= 'y' ;
                iTestState = PROC_MP_TEST ;
            }
            iTestReturnBool = 0 ;
            cTestReturn[1] = 0x0d ;
            vRS232_Write((unsigned char *)cTestReturn,(long)2);
        }else if ( iTestProcess == 3 ){
            if(iTestReturnBool==1)
                iTestState = PROC_EXIT;
            else if(iTestReturnBool==2)
                iTestState = PROC_MP_TEST_SELECT ;
        }
            ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_HIDE);
            ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_HIDE);
            iTestCount = 0 ;
            iTestTimeOut = 0 ;
            return ;
    }

}

void vTest_ProductNo( void )
{
    unsigned char ubBuf[128] ;
    long ulBytes ;

    HWND hInputWnd  = GetDlgItem( hMainWnd , IDC_EDIT_NO ) ;

    if ( iTestCount == 0 )
    {
        Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_NO ) , TRUE ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_HIDE);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_HIDE);
        vStatus_Show("�п�J���~�Ǹ�") ;
        SetFocus( hInputWnd ) ;
        iTestTimeOut = 0 ;
    }

    iTestCount ++ ;

    {
        int iStrLen ;

        iStrLen = GetWindowTextLength( hInputWnd ) ;

        if ( iStrLen )
        {
            iTestTimeOut ++ ;

            if (( iTestTimeOut >= 500 )&& ( iStrLen == DEF_TEST_PRODUCT_MAX_LEN) )
            {
                TCHAR  edittxt[32];

                GetWindowText( hInputWnd , edittxt , iStrLen+1 ) ;
                Edit_Enable( hInputWnd , FALSE ) ;
                memcpy( cTestProductNo , edittxt , DEF_TEST_PRODUCT_MAX_LEN ) ;
                vStatus_Show( "�п�J���~�Ǹ�" ) ;
                iTestState = PROC_ETHERNET_MAC ;
                cTestProductNo[10]= 0x0d ;
 //               cTestProductNo[11]= 0x0a ;
                vRS232_Write((unsigned char *)cTestProductNo,(long)11);
                iTestCount = 0 ;
                iTestTimeOut = 0 ;
                iTestMacIndex = 0 ;
                return ;
            }
            if ( iTestTimeOut >= 1000 ) // 10 sec
            {
                SetWindowText( hInputWnd , "" ) ;
                iTestTimeOut = 0 ;
            }
        }
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "�п�J���~�Ǹ�" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_EthernetMAC( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("�п�J LAN9514 - MAC ���X") ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_STATIC_INPUT ) , "LAN9514 - MAC" ) ;
        Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , TRUE ) ;
        SetFocus( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) ) ;
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

    {
        int iStrLen ;

        iStrLen = GetWindowTextLength( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) ) ;
        if ( iStrLen )
        {
            iTestTimeOut ++ ;
            if ( iStrLen == DEF_TEST_MAC_MAX_LEN )
            {
                TCHAR  edittxt[32];

                GetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , edittxt , iStrLen+1 ) ;
                Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , FALSE ) ;
                memcpy( cTestMacNum[0] , edittxt , DEF_TEST_MAC_MAX_LEN ) ;
                vStatus_Show( "LAN9514 - MAC OK" ) ;
                iTestState = PROC_ETHERNET_MAC1 ;
                cTestMacNum[0][12]= 0x0d ;
                vRS232_Write((unsigned char *)cTestMacNum[0],(long)13);
                iTestCount = 0 ;
                iTestTimeOut = 0 ;
                iTestMacIndex = 0 ;
                return ;
            }
            if ( iTestTimeOut >= 1000 ) // 10 sec
            {
                SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT ) , "" ) ;
                iTestTimeOut = 0 ;
            }
        }
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "�п�J LAN9514 - MAC ���X" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_EthernetMAC1( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("�ЦA��J IMX6 - MAC ���X") ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_STATIC_INPUT1 ) , "IMX6 - MAC" ) ;
        Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , TRUE ) ;
        SetFocus( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) ) ;
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

   {
        int iStrLen ;

        iStrLen = GetWindowTextLength( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) ) ;
        if ( iStrLen )
        {
            iTestTimeOut ++ ;
            if ( iStrLen == DEF_TEST_MAC_MAX_LEN )
            {
                TCHAR  edittxt[32];

                GetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , edittxt , iStrLen+1 ) ;
                Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , FALSE ) ;
                memcpy( cTestMacNum[1] , edittxt , DEF_TEST_MAC_MAX_LEN ) ;
                vStatus_Show( "IMX6 - MAC OK" ) ;
                iTestState = PROC_MP_TEST ;
                cTestMacNum[1][12]= 0x0d ;
                vRS232_Write((unsigned char *)cTestMacNum[1],(long)13);
                iTestCount = 0 ;
                iTestTimeOut = 0 ;
                iTestMacIndex = 0 ;
                return ;
            }
            if ( iTestTimeOut >= 1000 ) // 10 sec
            {
                SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT1 ) , "" ) ;
                iTestTimeOut = 0 ;
            }
        }
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "�ЦA��J IMX6 - MAC ���X" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_MP_TEST( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("���ݶi�J MP TEST �{��") ;
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

    if ( stcTestCheckData[1].iState  )
    {
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_RESTART ) , "�۰ʴ���" ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_SHOW);
        iTestState = PROC_MP_TEST_SELECT ;
        iTestCount = 0 ;
        return;
    }else if ( stcTestCheckData[0].iState  )
    {
        stcTestCheckData[0].iState = 0 ;
        iTestState = PROC_EXIT ;
        iTestCount = 0 ;
        vStatus_Show( "R_Num ��J����, �Э��s����!!!!" ) ;
        return;
    }
   if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "���ݶi�J MP TEST �{��" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }

}

void vTest_MP_TEST_Select( void )
{
    if ( iTestCount == 0 )
    {
        if ( ! iTestAuto ){
            vStatus_Show("�п�J MP TEST ���� (�Ҧp, 00 : �۰ʴ���)") ;
            SetWindowText( GetDlgItem( hMainWnd , IDC_STATIC_INPUT2 ) , "���ն���" ) ;
            SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "" ) ;
            Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , TRUE ) ;
            SetFocus( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) ) ;
        }
        stcTestCheckData[2].iState = 0;     //reinitial
        stcTestCheckData[3].iState = 0;     //reinitial
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

   {
        int iStrLen ;

        iStrLen = GetWindowTextLength( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) ) ;
        if ( iStrLen )
        {
            iTestTimeOut ++ ;
            if ( iStrLen == DEF_TEST_NUMBER_LEN )
            {
                TCHAR  edittxt[32];

                GetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , edittxt , iStrLen+1 ) ;
                ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_HIDE);
                Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , FALSE ) ;
                memcpy( cTestNum , edittxt , DEF_TEST_NUMBER_LEN ) ;
                vStatus_Show( "START TEST .. ." ) ;
                cTestNum[2]= 0x0d ;
                vRS232_Write((unsigned char *)cTestNum,(long)3);
                Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , FALSE ) ;
                if (( cTestNum[0] == 'q' ) && ( cTestNum[1] == 'q' ))
                    iTestState = PROC_EXIT ;
                else
                    iTestState = PROC_MP_TEST_TESTING ;
                iTestCount = 0 ;
                iTestTimeOut = 0 ;
                return ;
            }
            if ( iTestTimeOut >= 1500 )
            {
                SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "" ) ;
                iTestTimeOut = 0 ;
            }
        }else if ( stcTestCheckData[19].iState ){
            stcTestCheckData[19].iState = 0 ;
            iTestState = PROC_MP_TEST_TESTING  ;
            iTestCount = 0 ;
            iTestTimeOut = 0 ;
            Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , FALSE ) ;
            return ;
        }
    }
    if(iTestReturnBool==3)
    {
        iTestReturnBool = 0 ;
        cTestNum[0]= '0' ;
        cTestNum[1]= '0' ;
        cTestNum[2]= 0x0d ;
        vRS232_Write((unsigned char *)cTestNum,(long)3);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_HIDE);
        Edit_Enable( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , FALSE ) ;
        iTestState = PROC_MP_TEST_TESTING;
        iTestCount = 0 ;
        iTestTimeOut = 0 ;
        return ;
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "�п�J MP TEST ���� (�Ҧp, 00 : �۰ʴ���)" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_MP_TEST_testing( void )
{
    if ( iTestCount == 0 )
    {
        vStatus_Show("���դ�...") ;
        iTestTimeOut = 0 ;
        iTestItem = 0;
        iTestShowItem = 0;
    }

    iTestCount++ ;

    if ( stcTestCheckData[6].iState  )            iTestItem = 1 ;
    if ( stcTestCheckData[7].iState  )            iTestItem = 2 ;
    if ( stcTestCheckData[8].iState  )            iTestItem = 3 ;
    if ( stcTestCheckData[9].iState  )            iTestItem = 4 ;
    if ( stcTestCheckData[10].iState )            iTestItem = 5 ;
    if ( stcTestCheckData[11].iState )            iTestItem = 6 ;
    if ( stcTestCheckData[12].iState )            iTestItem = 7 ;
    if ( stcTestCheckData[13].iState )            iTestItem = 8 ;
    if ( stcTestCheckData[14].iState )            iTestItem = 9 ;
    if ( stcTestCheckData[15].iState )            iTestItem = 10 ;
    if ( stcTestCheckData[16].iState )            iTestItem = 11 ;
    if ( stcTestCheckData[17].iState )            iTestItem = 12 ;
    if ( stcTestCheckData[18].iState )            iTestItem = 13 ;
    if ( stcTestCheckData[20].iState )            iTestAuto = 1 ;

    if ((iTestItem > 0) && ( iTestShowItem == 0)){
            iTestShowItem = 1 ;
            switch (iTestItem) {
                case 1 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "LED ����" ) ;
                    break;
                case 2 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "GPIO ����" ) ;
                    break;
                case 3 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "AUDIO ����" ) ;
                    break;
                case 4 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "LVDS ����" ) ;
                    break;
                case 5 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "3AXIS ����" ) ;
                    break;
                case 6 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "TOUCHPANEL ����" ) ;
                    break;
                case 7 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "USB ����" ) ;
                    break;
                case 8 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "WIFI ����" ) ;
                    break;
                case 9 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "ETH0 ����" ) ;
                    break;
                case 10 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "ETH1 ����" ) ;
                    break;
                case 11 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "RS232 ����" ) ;
                    break;
                case 12 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "DDR ����" ) ;
                    break;
                case 13 :
                    SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "SDCARD ����" ) ;
                    break;
                default :
                    break;
            }
    }


    if ( stcTestCheckData[2].iState  )
    {
        iTestState = PROC_MP_TEST_RESULT ;
        iTestCount = 0 ;
        return ;
    }else if ( stcTestCheckData[3].iState  )
    {
        iTestState = PROC_MP_TEST_OK ;
        iTestCount = 0 ;
        return ;
    }

   if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "���դ�..." ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }

}

void vTest_MP_TEST_Result( void )
{
    if ( iTestCount == 0 )
    {
        iTestReturnBool = 0 ;
        vStatus_Show("�ЧP�w���յ��G") ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_FAIL ) , "FAIL" ) ;
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_YES_NO_OK ) , "PASS" ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_SHOW);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_SHOW);
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;
    if ( iTestReturnBool )
    {
        if(iTestReturnBool==1)      cTestReturn[0]= 'n' ;
        else if(iTestReturnBool==2) cTestReturn[0]= 'y' ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_FAIL), SW_HIDE);
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_YES_NO_OK), SW_HIDE);
        cTestReturn[1]= 0x0d ;
        vRS232_Write((unsigned char *)cTestReturn,(long)2);
        iTestState = PROC_MP_TEST_OK ;
        iTestCount = 0 ;
        iTestTimeOut = 0 ;
        return ;
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "�ЧP�w���յ��G" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_MP_TEST_ok( void )
{
    if ( iTestCount == 0 )
    {
        iTestReturnBool = 0 ;
        iTestResult = 0 ;
        iTestFinish = 0 ;
        vStatus_Show("���է���") ;
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

    if ( stcTestCheckData[4].iState  ){
            stcTestCheckData[4].iState = 0;
            vStatus_Show( "���է��� : PASS" ) ;
            iTestFinish = 1;
            iTestResult = 1;
    }else if( stcTestCheckData[5].iState  ){
            stcTestCheckData[5].iState = 0;
            vStatus_Show( "���է��� : FAIL" ) ;
            iTestFinish = 1;
            iTestResult = 0;
    }

    if (iTestFinish){
            stcTestCheckData[19].iState = 0 ;
            iTestState = PROC_MP_TEST_SHOW ;
            iTestCount = 0 ;
            return ;
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "���է���" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_MP_TEST_show( void )
{
    int iLoop ;

    if ( iTestCount == 0 )
    {
        vStatus_Show("���") ;
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

    if(iTestItem > 0){
        vReport_SetInfo(iTestItem-1,iTestResult? "PASS":"FAIL");
        iTestEnd = 1 ;
    }

    if (iTestEnd){
            if( (iTestItem == 13) && iTestAuto ){
                vStatus_Show( " MP ���դw����, �Ч󴫤U�@��PCBA" ) ;
                SetWindowText( GetDlgItem( hMainWnd , IDC_EDIT_INPUT2 ) , "" ) ;
                iTestState = PROC_EXIT ;
            }else{
                for ( iLoop = 6 ; iLoop < 19 ; iLoop ++ )
                    stcTestCheckData[iLoop].iState = 0 ;
                iTestState = PROC_MP_TEST_SELECT ;
            }
            iTestCount = 0 ;
            iTestTimeOut = 0 ;
            return ;
    }

    if ( ( iTestCount & 0xF) == 0x00 )
    {
        vStatus_Show( "���" ) ;
    }
    if ( ( iTestCount & 0xF) == 0x8 )
    {
        vStatus_Show( "" ) ;
    }
}

void vTest_Exit( void )
{
    if ( iTestCount == 0 )
    {
        SetWindowText( GetDlgItem( hMainWnd , IDC_BUTTON_RESTART ) , "���U�@�x" ) ;
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_SHOW);
        iTestTimeOut = 0 ;
    }

    iTestCount++ ;

    if( stcTestCheckData[22].iState  ){
        stcTestCheckData[22].iState = 0 ;
        iTestSavefile = 1;
    }
    if( stcTestCheckData[23].iState  ){
        stcTestCheckData[23].iState = 0 ;
        iTestSavefile = 0;
    }

    if(iTestReturnBool==3)
    {
        ShowWindow(GetDlgItem(hMainWnd, IDC_BUTTON_RESTART), SW_HIDE);

        iTestState = PROC_DATA_INIT;
        iTestCount = 0 ;
        iTestTimeOut = 0 ;
        return ;
    }
}

void vTest_Process( void )
{
       /* {
        char cTemp[32] ;
        sprintf(cTemp,"%d",iTestState) ;
        vStatus_Show( cTemp ) ;
    }*/
    switch( iTestState )
    {
        case PROC_RS232_START    : vTest_RS232Start( ) ;        break ;
        case PROC_DATA_INIT      : vTest_DataInit( ) ;          break ;
        case PROC_CONNECT_PCBA   : vTest_ConnectPCBA( ) ;       break ;
        case PROC_LOGIN          : vTest_login( ) ;             break ;
        case PROC_START_MP       : vTest_StartMp( ) ;           break ;
        case PROC_PRODUCT_NO     : vTest_ProductNo( ) ;         break ;
        case PROC_ETHERNET_MAC   : vTest_EthernetMAC( ) ;       break ;
        case PROC_ETHERNET_MAC1  : vTest_EthernetMAC1( ) ;      break ;
        case PROC_MP_TEST        : vTest_MP_TEST( ) ;           break ;
        case PROC_MP_TEST_SELECT : vTest_MP_TEST_Select( ) ;    break ;
        case PROC_MP_TEST_TESTING: vTest_MP_TEST_testing( ) ;   break ;
        case PROC_MP_TEST_RESULT : vTest_MP_TEST_Result( ) ;    break ;
        case PROC_MP_TEST_OK     : vTest_MP_TEST_ok( ) ;        break ;
        case PROC_MP_TEST_SHOW   : vTest_MP_TEST_show( );       break ;
        case PROC_EXIT           : vTest_Exit( ) ;              break ;
        default :
            iTestState = 0 ;
            iTestCount = 0 ;
            break ;
    }
/*
    if ( iMainRS232StartOrStop == 0 )
    {MAXDWORD
        if ( (!(iTestState == PROC_EXIT)) && (!(iTestState == PROC_RS232_START)) )
        {
            vStatus_Show( "UART ���_" ) ;
            iTestState = PROC_EXIT ;
            iTestCount = 0 ;
        }
    }
*/
}
