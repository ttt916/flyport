#include "taskFlyport.h"
#include "grovelib.h"
 
char phonenumber[25] = "+886935865528"; // Remember to use INTERNATIONAL PREFIX
char smsTextBuff[20];// = " test ";
char smsTextBuff2[20];// = " test2 ";

TCP_SOCKET sockTcp;
char inBuff[1500];
BOOL sendRequest = FALSE;
int lenTemp = 0;

double val;	
double elc;

double val2;	
double elc2;

double ans[2];
char warning[][20];
int tag=0;

int delaytime = 1;

void Send()
{
	// Check for Serial commands
		
        vTaskDelay(20);
	    int len = UARTBufferSize(1);
	    if(len > 160)
		len = 160;
		char sendtext[100];
		
		printf("hi2 %s",warning[0]);
		sprintf(sendtext,"AN1:%f ans1:%s\n AN2:%f ans2:%s\n",ans[0],warning[0],ans[1],warning[1]);
	    
		UARTRead(1,sendtext, len);
	  //  smsTextBuff[len] = '\0';
            UARTWrite(1, "Sending SMS...");
	    // Send SMS:
	    SMSSend(phonenumber,sendtext, FALSE);
 
	    while(LastExecStat() == OP_EXECUTION)
		vTaskDelay(1);
 
	    if(LastExecStat() == OP_SUCCESS)
		UARTWrite(1, "SMS sent!\r\n");
	    else
		UARTWrite(1, "SMS NOT sent!\r\n");
	   
}

void sendgoogle()
{
	UARTWrite(1, "start sendgoogle\r\n");
	int actionState = 1;
	int len=0,i=0,j=0,z=0;	 
	
	//for(i=0;i<2;i++){
	while(tag!=0){
		switch(actionState)
		{
			// Step #0: IDLE operation
			case 0:
			// Insert here your IDLE code (if needed...) 
			break;
 
			// Step #1: APN Config
			case 1:
			UARTWrite(1, "Setup APN params\r\n");
			APNConfig("internet","usernameAPN","passwordAPN",DYNAMIC_IP,DYNAMIC_IP,DYNAMIC_IP);
			vTaskDelay(200);
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(200);
			// Choose next step: 
			if(LastExecStat() != OP_SUCCESS){
				actionState = 1; // Repeat from case 1
				UARTWrite(1, "connection failed\r\n");
			}else{
				actionState++; // execute next step
				UARTWrite(1, "connection success\r\n");
			}
			break;
	
			// Step #2: Connect socket to TCP Server
			case 2:
			UARTWrite(1, "Connecting to TCP Server...");
			TCPClientOpen(&sockTcp, "213.186.33.19","80");
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(10);
			vTaskDelay(200);
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors on TCPClientOpen function!\r\n");	
				actionState = 7; // Close socket and try again	
			}	
			else
			{
				UARTWrite(1, "\r\n TCPClientOpen OK \r\n");
				UARTWrite(1, "Socket Number: ");
				sprintf(inBuff, "%d\r\n", sockTcp.number);
				UARTWrite(1, inBuff);	
				actionState++; // execute next step
			}
			break;
	
			// Step #3: Update TCP_SOCKET Status
			case 3:
			UARTWrite(1, "Updating TCP_SOCKET Status...");
			TCPStatus(&sockTcp);
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(1);
			vTaskDelay(20);
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors on updating TCPStatus!\r\n");	
				actionState = 7; // Close socket and try again	
			}
			else
			{
				UARTWrite(1, "\r\n TCP Socket Status:\r\n");
				sprintf(inBuff, "Status: %d\r\n", sockTcp.status);
				UARTWrite(1, inBuff);
				sprintf(inBuff, "RxLen: %d\r\n", sockTcp.rxLen);
				UARTWrite(1, inBuff);	
				actionState++; // execute next step
			}
			break;
	
			// Step #4: Send TCP DATA (HTTP GET Request)------------------------------傳送網頁指令
			case 4:
			UARTWrite(1, "Sending data...");
	
			//char data[] = "GET /custom_openpicus/logo_openpicus.png HTTP/1.1\r\nHost: wiki.openpicus.com\r\n\r\n\r\n"; 
			//TCPWrite(&sockTcp, data, strlen(data));
		
			char tmpString[1024];
			char payload_1[50] ;//= "entry.0.single=";
			char payload_2[50] ;//= "&entry.1.single=";
			char payload_3[50] ;
			char payload_4[50] ;
			char temp[3]={0};
		
			
			//char * payload_4 = "&submit=Submit";
				//	i=tag-1;
					
					//printf("hi %d\n",i);
					vTaskDelay(20);
					sprintf(payload_1,"&data1=%lf",ans[0]);
					sprintf(payload_2,"&data2=%lf",ans[1]);							
					sprintf(payload_3,"&data3=%s","flyport#1");
					if(ans[0]<394){
						sprintf(payload_4,"&data4=%s",warning[i]);
						}else sprintf(payload_4,"&data4=%s",warning[i]);
				
						
						
		
				sprintf(tmpString,"POST /pushingbox/pushingbox?devid=vA5A13446C8503AB HTTP/1.1\r\nHost: api.pushingbox.com\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s%s%s%s%s%s%s\r\n",
				strlen(payload_1)+strlen(payload_2)+strlen(payload_3)+strlen(payload_4),payload_1,payload_2,payload_3,payload_4);
			
		
			UARTWrite(1,tmpString);
			TCPWrite(&sockTcp, tmpString, strlen(tmpString));
			
		
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(1);
			vTaskDelay(20);
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors sending TCP data!\r\n");	
				actionState = 7; // Close socket and try again	
			}	
			else
			{
				UARTWrite(1, "\r\n TCP data sent:\r\n");
				sprintf(inBuff, "Status: %d\r\n\r\n\r\n", sockTcp.status);
				UARTWrite(1, inBuff);	
				actionState++; // execute next step
			}
			
			break;
 
			// Step #5: Update TCP_SOCKET Status
			case 5:
			TCPStatus(&sockTcp);
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(1);
			vTaskDelay(20);
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors on TCPStatus!\r\n");	
				actionState = 7; // Close socket and try again
			}	
			else
			{	
				if(sockTcp.rxLen > 0)
				actionState++; // execute next step
				else
				{
				// Action completed (no more data received...), close socket:
				actionState = 7; // force step #7
				}
			}
			break;
 
			// Step #6: Read until end of data...
			case 6:
			lenTemp = sockTcp.rxLen;
			TCPRead(&sockTcp, inBuff, lenTemp);
			inBuff[lenTemp] = '\0';
 
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(1);
			vTaskDelay(20);
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors on reading TCP buffer!\r\n");	
				actionState = 7; // Close socket and try again
			}	
			else
			{
				UARTWrite(1, inBuff); // Write data to serial Monitor	
				actionState = 5; // Update again TCP_SOCKET Status
			}
			break;
 
			// Step #7: Close TCP_SOCKET
			case 7:
			UARTWrite(1, "\r\n\r\nClosing socket...");
			TCPClientClose(&sockTcp);
 
			while(LastExecStat() == OP_EXECUTION)
				vTaskDelay(1);
			vTaskDelay(20);
			
			if(LastExecStat() != OP_SUCCESS)
			{
				UARTWrite(1, "Errors on TCPClientClose!\r\n");	
				actionState = 1; // Repeat from case 1	
			}	
			else
			{
				UARTWrite(1, "Socket Closed!\r\n");	
				tag=0;
				actionState = 1;
			}
			break;
		}//end switch
		}//end while
	//}//end for
}//end void sendgoogle


void FlyportTask()
{
    sockTcp.number = INVALID_SOCKET;
 
    vTaskDelay(20);
    UARTWrite(1,"Flyport Task Started...\r\n");
 
    // Wait for GSM Connection successfull
    while((LastConnStatus() != REG_SUCCESS)&&(LastConnStatus() != ROAMING))
    {
    	vTaskDelay(1);
    }
    vTaskDelay(20);
    UARTWrite(1,"Flyport registered on network!\r\n");
    // end net connect
	
	//char str[20];				// String to write the potentiometer values

	//double v;
	vTaskDelay(50);
	UARTWrite(1,"Welcome to GROVE NEST example!\r\n");
 
	// GROVE board
	void *board = new(GroveNest);
 
	// GROVE devices	
	void *myPot = new(An_i);
	void *myPot2 = new(An_i);
	attachToBoard(board, myPot, AN1);
	attachToBoard(board, myPot2, AN2);
 
	//	Main loop
	while(1)
	{
		int flag=1;
		
		if(flag==1){
		int i;
			vTaskDelay(5000);
		flag=1;
		tag=2;
		val=(rand()%100)+1;
		
		//val = get(myPot);//*(3.35/1023.0);              // Reading potentiometer value
		//v = val*(5/1023);
		//elc = val;//660*(220+660)/220*1000;
		ans[0] = /*(*/val;//660*(220+660)/220*1000-4)/16*500*0.79;
		//ans = val/660*(220+660)/220;
		printf("\r\nVol: %f\r\n",val);
		printf("\r\nelc: %f\r\n",elc);
		printf("\r\nValue: %f\r\n",ans[0]);
		
		vTaskDelay(20);
		
		val2 = get(myPot2);//*(3.35/1023.0);              // Reading potentiometer value
		//v = val*(5/1023);
		elc2 = val2;//660*(220+660)/220*1000;
		ans[1] = /*(*/val2;//660*(220+660)/220*1000-4)/16*500*0.79;
		//ans = val/660*(220+660)/220;
		printf("\r\nVol2: %f\r\n",val2);
		printf("\r\nelc2: %f\r\n",elc2);
		printf("\r\nValue2: %f\r\n",ans[1]);
		vTaskDelay(20);
		int a=tag-1;
		int b;
		
		printf("hi3 %f\n",ans[a]);
		for(b=0;b<=a;b++){
			
		if(ans[b] > 100 )
		{
			printf("12313123\n");
			sprintf(warning[b], "warning");
			Send();
		}else{
			sprintf(warning[b], "normal");
		}
		}
		//UARTWrite(1, warning);
		
		vTaskDelay(20);
		
		
			sendgoogle();
			
			
			flag=0;
		//UARTWrite(1, str);
		}
		
	}
}














