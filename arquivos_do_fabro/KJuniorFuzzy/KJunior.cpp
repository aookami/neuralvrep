#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>
#include "rightMotor.h"
#include "leftMotor.h"

extern "C" {
	#include "extApi.h"
}

int main(int argc, const char * argv[]) {

	std::string s;

	int portNb, leftMotorHandle, rightMotorHandle;
	int sensors [6] = {0, 0, 0, 0, 0, 0};
	float distances [6] = {0, 0, 0, 0, 0, 0};


	//check if there are correct quantity of parameters
	if (argc>=10)
	{
		portNb=atoi(argv[1]);
		leftMotorHandle=atoi(argv[2]);
		rightMotorHandle=atoi(argv[3]);
		//sensorHandle=atoi(argv[4], argv[4], argv[4]);
		for(int i = 0; i < 6; i++){
			sensors[i] = atoi(argv[i+4]);
			printf("Getting sensor %d\n", sensors[i]);
		}
	}
	else
	{
		printf("Indicate following arguments: 'portNumber leftMotorHandle rightMotorHandle sensors'!\n");
		extApi_sleepMs(5000);
		return 0;
	}

	simxInt clientID=simxStart((simxChar*)"127.0.0.1",portNb,true,true,2000,5);

	//Creating and opening console
	simxInt consoleHandle = -1;
	simxInt consoleID = simxAuxiliaryConsoleOpen(clientID,"Console Window for Fuzzy Project",10000,6,NULL,NULL,NULL,NULL, &consoleHandle, simx_opmode_blocking);
	//Printing something on the console
	simxInt returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"Starting simulation",simx_opmode_blocking);

	leftMotor* motorE = new leftMotor();
	rightMotor* motorD = new rightMotor();

	if (clientID!=-1)
	{
		//int driveBackStartTime=-99000;
		float motorSpeeds[2];
		unsigned char readValue = 0;
		float readDistance[3] = {0, 0, 0};

		returnNumber = simxReadProximitySensor(clientID,sensors[2],&readValue,&(readDistance[0]),NULL,NULL,simx_opmode_streaming);
		while (simxGetConnectionId(clientID)!=-1)
		{

			/*

			 Reding values from sensors

			 */
			for(int i=0; i<6; i++){
				returnNumber = simxReadProximitySensor(clientID,sensors[i],&readValue,&(readDistance[0]),NULL,NULL,simx_opmode_buffer);

				//calculating distance sqrt(x^2+y^2+z^2)
				distances[i] = sqrt(pow(readDistance[0], 2.0) + pow(readDistance[1], 2.0) + pow(readDistance[2], 2.0))*3200;

				if(distances[i]>999)
					distances[i] = 999;
				//getting complement of it (1000-x)
				//distances[i] = 999 - distances[i];
			}

			/*

			 Printing calculated distances

			 */

			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n\n",simx_opmode_blocking);

			for(int i=0; i<6; i++){
				//converting from int to String
				std::string s = std::to_string(distances[i]);
				//converting from string to char*
				char const *pchar = s.c_str();  //use char const* as target type
				returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pchar,simx_opmode_blocking);

				//break line
				returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);
			}

			//getting the maximum (worst) value
			float maxEsquerda	= 0;
			float maxFrente		= 0;
			float maxDireita	= 0;
			//frente
			maxFrente = distances[2];
			//esquerda
			if(distances[0] > distances[1])
				maxEsquerda = distances[0];
			else
				maxEsquerda = distances[1];
			//direita
			if(distances[3] > distances[4])
				maxDireita = distances[3];
			else
				maxDireita = distances[4];



			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n ==Valores Max==\n",simx_opmode_blocking);

			s = std::to_string(maxEsquerda);
			//converting from string to char*
			char const *pcharMaxEsq = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharMaxEsq,simx_opmode_blocking);
			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);
			s = std::to_string(maxFrente);
			//converting from string to char*
			char const *pcharMaxFre = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharMaxFre,simx_opmode_blocking);
			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);
			s = std::to_string(maxDireita);
			//converting from string to char*
			char const *pcharMaxDir = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharMaxDir,simx_opmode_blocking);

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);

			/*

			 Applying Fuzzy rules

			 */
			float valueMotorE = motorE->makeInference(maxEsquerda, maxFrente, maxDireita); //200,200,900
			float valueMotorD = motorD->makeInference(maxEsquerda, maxFrente, maxDireita);//maxEsquerda, maxFrente, maxDireita);
			//valueMotorE = valueMotorE;//*0.31415;
			//valueMotorD = valueMotorD;//*0.31415;

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n ==Valor antes motores==\n",simx_opmode_blocking);

			s = std::to_string(valueMotorE);
			//converting from string to char*
			char const *pcharE = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharE,simx_opmode_blocking);

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);

			s = std::to_string(valueMotorD);
			//converting from string to char*
			char const *pcharD = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharD,simx_opmode_blocking);

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);

			/*

			 Writing motor speeds back to simulator

			 */

			if(valueMotorD == 0.0)
				valueMotorD = 7.0;
			if(valueMotorE == 0.0)
				valueMotorE = 7.0;




			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n ==Valor depois motores==\n",simx_opmode_blocking);

			s = std::to_string(valueMotorE);
			//converting from string to char*
			char const *pcharE2 = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharE2,simx_opmode_blocking);

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);

			s = std::to_string(valueMotorD);
			//converting from string to char*
			char const *pcharD2 = s.c_str();  //use char const* as target type
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,pcharD2,simx_opmode_blocking);

			//break line
			returnNumber = simxAuxiliaryConsolePrint(clientID,consoleHandle,"\n",simx_opmode_blocking);





			simxSetJointTargetVelocity(clientID,leftMotorHandle,valueMotorE, simx_opmode_streaming);
			simxSetJointTargetVelocity(clientID,rightMotorHandle,valueMotorD, simx_opmode_streaming);


			//writing values in the

			simxInt simxSetJointTargetVelocity(simxInt clientID,simxInt jointHandle,simxFloat targetVelocity,simxInt operationMode);

			if (simxReadProximitySensor(clientID,sensors[0],NULL,NULL,NULL,NULL,simx_opmode_streaming)==simx_return_ok)
			{ // We succeeded at reading the proximity sensor
				int simulationTime=simxGetLastCmdTime(clientID);
			}
			extApi_sleepMs(5);
		}
		simxFinish(clientID);
	}



    return 0;
}

