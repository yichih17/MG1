#include<iostream>
#include<vector>
#include<random>

#define radius_eNB 1732
#define RunTime 1000000
#define UELevel 10
using namespace std;
int LTECQIRange[] = { 1732, 1511, 1325, 1162, 1019, 894, 784, 688, 623, 565, 512, 449, 407, 357, 303 };

struct UE
{
	double Coor_X;
	double Coor_Y;
	double pktsize;
	double bitrate;
	int CQI;
};

void uniformdistribution(UE* u)
{
	std::random_device rd;							//integer random number generator that produces non-deterministic random numbers. 
	std::mt19937 gen(rd());							//Mersenne Twister 19937 generator, generate a random number seed
	std::uniform_real_distribution<> theta(0, 360);	//definition of a uniform distribution range, a random number between 0 and 360
	std::uniform_real_distribution<> k(0, 1);		//definition of a uniform distribution range, a random number between 0 and 1
	double r = radius_eNB * sqrt(k(gen));			//random a angle and random a radius, to gennerate a coordinate for UE
	double angel = (theta(gen));
	u->Coor_X = r * std::sin(angel);
	u->Coor_Y = r * std::cos(angel);
}

double getDistance(double coor_x, double coor_y) { return sqrt(pow(coor_x, 2) + pow(coor_y, 2)); }

int get_CQI(UE* u)
{
	double distance = getDistance(u->Coor_X, u->Coor_Y);
	int CQI = 0;
	for (int i = 0; i < 15; i++)
	{
		if (distance <= LTECQIRange[i])
			CQI++;
		else
			break;
	}
	return CQI;
}

double CQIEfficiency(int CQI)
{
	double efficiency = 0.0;
	switch (CQI)
	{
	case 1:
		efficiency = 0.1523;
		break;
	case 2:
		efficiency = 0.2344;
		break;
	case 3:
		efficiency = 0.3770;
		break;
	case 4:
		efficiency = 0.6016;
		break;
	case 5:
		efficiency = 0.8770;
		break;
	case 6:
		efficiency = 1.1758;
		break;
	case 7:
		efficiency = 1.4766;
		break;
	case 8:
		efficiency = 1.9141;
		break;
	case 9:
		efficiency = 2.4063;
		break;
	case 10:
		efficiency = 2.7305;
		break;
	case 11:
		efficiency = 3.3223;
		break;
	case 12:
		efficiency = 3.9023;
		break;
	case 13:
		efficiency = 4.5234;
		break;
	case 14:
		efficiency = 5.1152;
		break;
	case 15:
		efficiency = 5.5547;
		break;
	default:
		break;
	}
	return efficiency;
}

int main()
{
	double capacity[15];
	for (int i = 0; i < 15; i++)
	{
		capacity[i] = 144 * CQIEfficiency(i + 1) * 100;
	}

	int count[UELevel] = { 0 };

	double result[UELevel][2] = { 0 };
	for (int times = 0; times < RunTime; times++)
	{
		//cout << times;
		for (int i = 0; i < UELevel; i++)
		{
			int UEnumber = (i + 1) * 5;
			//cout << " " << UEnumber;
			//if (i == 4)
			//	cout << endl;
			vector <UE> list;
			for (int j = 0; j < UEnumber; j++)
			{
				UE temp;
				temp.bitrate = 100;
				temp.pktsize = 8000;
				uniformdistribution(&temp);
				temp.CQI = get_CQI(&temp);
				list.push_back(temp);
			}
						
			double lambda = UEnumber * 0.0375;
			double Xj = 0.0;
			double Xj2 = 0.0;
			double weight_i = (double)1 / (double)UEnumber;
			for (int i = 0; i < UEnumber; i++)
			{				
				double Xij = list[i].pktsize / capacity[list[i].CQI];
				Xj += Xij * weight_i;
				Xj2 += pow(Xij, 2) * weight_i;
			}
			double rho = lambda * Xj;
			result[i][0] += rho;
			if (rho < 1)
			{
				result[i][1] += Xj + lambda * Xj2 / (1 - lambda * Xj);
				count[i]++;
			}				
		}
	}

	for (int i = 0; i < UELevel; i++)
	{
		result[i][0] /= count[i];
		result[i][1] /= count[i];
		cout << result[i][0] << "," << result[i][1] << endl;
	}
	
	return 0;
}