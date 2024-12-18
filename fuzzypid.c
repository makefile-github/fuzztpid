#include "fuzzypid.h"
#include <unistd.h>

/**********************************************************
模糊化
***********************************************************/

// left,center,right 构成三角形triangle
// 返回x对应的隶属度
float membershipFunc(float left, float center, float right, float x)
{
	if (x >= left && x <= center)
	{
		//防止分母为0
		return (left != center)?((x - left) / (center - left)):1;
	}
	else if (x > center && x <= right)
	{
		return (center != right)?((right - x) / (right - center)):1;
	}
	return 0;

	// if (x <= left)
	// {
	// 	return left;
	// }
	// else if (x >= right)
	// {
	// 	return right;
	// }
	// else if (x >= left && x <= center)
	// {
	// 	return (x - left) / (center - left);
	// }
	// else if (x > center && x <= right)
	// {
	// 	return (right - x) / (right - center);
	// }
	// else	return 0;
}

// e & ec: e = input - target
// res 用来保存多个隶属度结果
void eMembership(float x, float *res, int n)
{
	char flag[7] = {0};
	float value[7] = {0};
	int i = 0, j = 0;

	if (ENB(x))
	{
		flag[0] = 1;
		value[0] = membershipFunc(e[0][0], e[0][1], e[0][2], x);
	}
	if (ENM(x))
	{
		flag[1] = 1;
		value[1] = membershipFunc(e[1][0], e[1][1], e[1][2], x);
	}
	if (ENS(x))
	{
		flag[2] = 1;
		value[2] = membershipFunc(e[2][0], e[2][1], e[2][2], x);
	}
	if (EZO(x))
	{
		flag[3] = 1;
		value[3] = membershipFunc(e[3][0], e[3][1], e[3][2], x);
	}
	if (EPS(x))
	{
		flag[4] = 1;
		value[4] = membershipFunc(e[4][0], e[4][1], e[4][2], x);
	}
	if (EPM(x))
	{
		flag[5] = 1;
		value[5] = membershipFunc(e[5][0], e[5][1], e[5][2], x);
	}
	if (EPB(x))
	{
		flag[6] = 1;
		value[6] = membershipFunc(e[6][0], e[6][1], e[6][2], x);
	}
	if (ENONE(x))
	{
		for (i = 0; i < n; i++)
		{
			res[i] = 0;
		}
	}

	for (i = 0; i < 7; i++)
	{
		if (flag[i] == 1)
		{
			res[j] = value[i];
			j++;
		}
	}
}

void ecMembership(float x, float *res, int n)
{
	char flag[7] = {0};
	float value[7] = {0};
	int i = 0, j = 0;

	if (ECNB(x))
	{
		flag[0] = 1;
		value[0] = membershipFunc(ec[0][0], ec[0][1], ec[0][2], x);
	}
	if (ECNM(x))
	{
		flag[1] = 1;
		value[1] = membershipFunc(ec[1][0], ec[1][1], ec[1][2], x);
	}
	if (ECNS(x))
	{
		flag[2] = 1;
		value[2] = membershipFunc(ec[2][0], ec[2][1], ec[2][2], x);
	}
	if (ECZO(x))
	{
		flag[3] = 1;
		value[3] = membershipFunc(ec[3][0], ec[3][1], ec[3][2], x);
	}
	if (ECPS(x))
	{
		flag[4] = 1;
		value[4] = membershipFunc(ec[4][0], ec[4][1], ec[4][2], x);
	}
	if (ECPM(x))
	{
		flag[5] = 1;
		value[5] = membershipFunc(ec[5][0], ec[5][1], ec[5][2], x);
	}
	if (ECPB(x))
	{
		flag[6] = 1;
		value[6] = membershipFunc(ec[6][0], ec[6][1], ec[6][2], x);
	}
	if (ECNONE(x))
	{
		for (i = 0; i < n; i++)
		{
			res[i] = 0;
		}
	}

	for (i = 0; i < 7; i++)
	{
		if (flag[i] == 1)
		{
			res[j] = value[i];
			j++;
		}
	}
}

/************************************************
规则判断
返回所有被触发规则的隶属度，以及对应的状态
最多同时触发9条规则
which表示使用P，I，D中哪一个的模糊规则
*************************************************/

/*enb enm ens ezo eps epm epb*/
float pRule[7][7] = {/*ecnb*/ {PB, PB, PM, PM, PS, ZO, ZO},
					 /*ecnm*/ {PB, PB, PM, PS, PS, ZO, NS},
					 /*ecns*/ {PM, PM, PM, PS, ZO, NS, NS},
					 /*eczo*/ {PM, PM, PS, ZO, NS, NM, NM},
					 /*ecps*/ {PS, PS, ZO, NS, NS, NM, NM},
					 /*ecpm*/ {PS, ZO, NS, NM, NM, NM, NB},
					 /*ecpb*/ {ZO, ZO, NM, NM, NM, NB, NB}};

/*enb enm ens ezo eps epm epb*/
float iRule[7][7] = {/*ecnb*/ {NB, NB, NM, NM, NS, ZO, ZO},
					 /*ecnm*/ {NB, NB, NM, NS, NS, ZO, ZO},
					 /*ecns*/ {NB, NM, NS, NS, ZO, PS, PS},
					 /*eczo*/ {NM, NM, NS, ZO, PS, PM, PM},
					 /*ecps*/ {NM, NS, ZO, PS, PS, PM, PB},
					 /*ecpm*/ {ZO, ZO, PS, PS, PM, PB, PB},
					 /*ecpb*/ {ZO, ZO, PS, PM, PM, PB, PB}};

/*enb enm ens ezo eps epm epb*/
float dRule[7][7] = {/*ecnb*/ {PS, NS, NB, NB, NB, NM, PS},
					 /*ecnm*/ {PS, NS, NB, NM, NM, NS, ZO},
					 /*ecns*/ {ZO, NS, NM, NM, NS, NS, ZO},
					 /*eczo*/ {ZO, NS, NS, NS, NS, NS, ZO},
					 /*ecps*/ {ZO, ZO, ZO, ZO, ZO, ZO, ZO},
					 /*ecpm*/ {PB, NS, PS, PS, PS, PS, PB},
					 /*ecpb*/ {PB, PM, PM, PM, PS, PS, PB}};

int isMatch(char i, char j, float xe, float xec)
{
	if (i == 0 && j == 0)
	{
		return ECNB(xec) && ENB(xe);
	}
	else if (i == 0 && j == 1)
	{
		return ECNB(xec) && ENM(xe);
	}
	else if (i == 0 && j == 2)
	{
		return ECNB(xec) && ENS(xe);
	}
	else if (i == 0 && j == 3)
	{
		return ECNB(xec) && EZO(xe);
	}
	else if (i == 0 && j == 4)
	{
		return ECNB(xec) && EPS(xe);
	}
	else if (i == 0 && j == 5)
	{
		return ECNB(xec) && EPM(xe);
	}
	else if (i == 0 && j == 6)
	{
		return ECNB(xec) && EPB(xe);
	}

	else if (i == 1 && j == 0)
	{
		return ECNM(xec) && ENB(xe);
	}
	else if (i == 1 && j == 1)
	{
		return ECNM(xec) && ENM(xe);
	}
	else if (i == 1 && j == 2)
	{
		return ECNM(xec) && ENS(xe);
	}
	else if (i == 1 && j == 3)
	{
		return ECNM(xec) && EZO(xe);
	}
	else if (i == 1 && j == 4)
	{
		return ECNM(xec) && EPS(xe);
	}
	else if (i == 1 && j == 5)
	{
		return ECNM(xec) && EPM(xe);
	}
	else if (i == 1 && j == 6)
	{
		return ECNM(xec) && EPB(xe);
	}

	else if (i == 2 && j == 0)
	{
		return ECNS(xec) && ENB(xe);
	}
	else if (i == 2 && j == 1)
	{
		return ECNS(xec) && ENM(xe);
	}
	else if (i == 2 && j == 2)
	{
		return ECNS(xec) && ENS(xe);
	}
	else if (i == 2 && j == 3)
	{
		return ECNS(xec) && EZO(xe);
	}
	else if (i == 2 && j == 4)
	{
		return ECNS(xec) && EPS(xe);
	}
	else if (i == 2 && j == 5)
	{
		return ECNS(xec) && EPM(xe);
	}
	else if (i == 2 && j == 6)
	{
		return ECNS(xec) && EPB(xe);
	}

	else if (i == 3 && j == 0)
	{
		return ECZO(xec) && ENB(xe);
	}
	else if (i == 3 && j == 1)
	{
		return ECZO(xec) && ENM(xe);
	}
	else if (i == 3 && j == 2)
	{
		return ECZO(xec) && ENS(xe);
	}
	else if (i == 3 && j == 3)
	{
		return ECZO(xec) && EZO(xe);
	}
	else if (i == 3 && j == 4)
	{
		return ECZO(xec) && EPS(xe);
	}
	else if (i == 3 && j == 5)
	{
		return ECZO(xec) && EPM(xe);
	}
	else if (i == 3 && j == 6)
	{
		return ECZO(xec) && EPB(xe);
	}

	else if (i == 4 && j == 0)
	{
		return ECPS(xec) && ENB(xe);
	}
	else if (i == 4 && j == 1)
	{
		return ECPS(xec) && ENM(xe);
	}
	else if (i == 4 && j == 2)
	{
		return ECPS(xec) && ENS(xe);
	}
	else if (i == 4 && j == 3)
	{
		return ECPS(xec) && EZO(xe);
	}
	else if (i == 4 && j == 4)
	{
		return ECPS(xec) && EPS(xe);
	}
	else if (i == 4 && j == 5)
	{
		return ECPS(xec) && EPM(xe);
	}
	else if (i == 4 && j == 6)
	{
		return ECPS(xec) && EPB(xe);
	}

	else if (i == 5 && j == 0)
	{
		return ECPM(xec) && ENB(xe);
	}
	else if (i == 5 && j == 1)
	{
		return ECPM(xec) && ENM(xe);
	}
	else if (i == 5 && j == 2)
	{
		return ECPM(xec) && ENS(xe);
	}
	else if (i == 5 && j == 3)
	{
		return ECPM(xec) && EZO(xe);
	}
	else if (i == 5 && j == 4)
	{
		return ECPM(xec) && EPS(xe);
	}
	else if (i == 5 && j == 5)
	{
		return ECPM(xec) && EPM(xe);
	}
	else if (i == 5 && j == 6)
	{
		return ECPM(xec) && EPB(xe);
	}

	else if (i == 6 && j == 0)
	{
		return ECPB(xec) && ENB(xe);
	}
	else if (i == 6 && j == 1)
	{
		return ECPB(xec) && ENM(xe);
	}
	else if (i == 6 && j == 2)
	{
		return ECPB(xec) && ENS(xe);
	}
	else if (i == 6 && j == 3)
	{
		return ECPB(xec) && EZO(xe);
	}
	else if (i == 6 && j == 4)
	{
		return ECPB(xec) && EPS(xe);
	}
	else if (i == 6 && j == 5)
	{
		return ECPB(xec) && EPM(xe);
	}
	else if (i == 6 && j == 6)
	{
		return ECPB(xec) && EPB(xe);
	}

	else
		return 0;
}

void PIDFuzzyRule(float xe, float xec, float res[][3], int which, int n)
{
	float temp[49][3] = {0};
	char flag[49] = {0};
	float(*p)[7] = NULL;
	int i = 0, j = 0, k = 0, cnt = 0;
	switch (which)
	{
	case P:
		p = pRule;
		break;
	case I:
		p = iRule;
		break;
	case D:
		p = dRule;
		break;
	default:
		break;
	}

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 7; j++)
		{
			if (isMatch(i, j, xe, xec))
			{
				flag[k] = 1;
				temp[k][0] = membershipFunc(e[j][0], e[j][1], e[j][2], xe);
				temp[k][1] = membershipFunc(ec[i][0], ec[i][1], ec[i][2], xec);
				temp[k][2] = p[i][j];
				//printf("%dmatch[%d][%d];cnt:%d;xe:%f;xec:%f;lvl:%f\n",which,i,j,k,temp[k][0],temp[k][1],temp[k][2]);
			}
			k++;
		}
	}
	// //未找到匹配区域处理
	// if (k == 0)
	// {
	// 	//极限值处理，也可以模糊控制不介入
	// }

	cnt = k;	//备份匹配数量
	k = 0;

	for (i = 0; i < 49; i++)
	{
		if (flag[i] == 1)
		{
			for (j = 0; j < 3; j++)
			{
				res[k][j] = temp[i][j];
			}
			k++;
			if(k == cnt)
			{
				break;
			}
		}
	}
}

/*****************************************************
解模糊
最大隶属度，最大值法
which 表示对P，I，D中的哪一种进行解模糊
******************************************************/

float getPartOfRes(float arr[], int n, int which)
{
	float(*p)[3] = NULL;
	int i = 0, j = 0, k = 0;
	switch (which)
	{
	case P:
		p = kp;
		break;
	case I:
		p = ki;
		break;
	case D:
		p = kd;
		break;
	default:
		break;
	}
	if (arr[2] == NB)
	{
		return p[0][2] - (p[0][2] - p[0][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == NM)
	{
		return p[1][2] - (p[1][2] - p[1][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == NS)
	{
		return p[2][2] - (p[2][2] - p[2][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == ZO)
	{
		return p[3][2] - (p[3][2] - p[3][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == PS)
	{
		return p[4][2] - (p[4][2] - p[4][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == PM)
	{
		return p[5][2] - (p[5][2] - p[5][1]) * (arr[0] * arr[1]);
	}
	else if (arr[2] == PB)
	{
		return p[6][2] - (p[6][2] - p[6][1]) * (arr[0] * arr[1]);
	}
	else
		return -1;
}

float defuzzy(float res[][3], int n, int which)
{
	int i = 0;
	float max = res[0][0] * res[0][1];
	int k = 0;
	float temp = 0;
	for (i = 1; i < 49; i++)	//n 变更成全部数据遍历
	{
		if ((res[i][0]) * (res[i][1]) > max)
		{
			max = res[i][0] * res[i][1];
		}
	}

	for (i = 0; i < 49; i++)	//n 变更成全部数据遍历
	{
		if ((res[i][0]) * (res[i][1]) - max <= 0.00001 && (res[i][0]) * (res[i][1]) - max >= -0.00001) // important no ==
		{
			temp += getPartOfRes(res[i], 3, which);
			k++;
		}
	}

	//防止除法分母为0
	if (k == 0)		
	{
		k = 1;
	}
	return temp / k;
}

float t = 5;													 // 0.04;
PARAM param[4] = {{0, 0, 0, 0, 0, 0, 1000, 0, 10, 10, 1, 1, 1}, //{ 0, 0, 0, 0, 0, 0, 4700, 10, 33.3, 3.3, 10, 10, 1 }
				  {0, 0, 0, 0, 0, 0, 1000, 0, 33.3, 3.3, 10, 10, 10},
				  {0, 0, 0, 0, 0, 0, 1000, 0, 33.3, 3.3, 10, 10, 10},
				  {0, 0, 0, 0, 0, 0, 1000, 0, 33.3, 3.3, 10, 10, 10}};

int outPut(float target, float cur, float kp, float ti, float td, int whichChannel)
{

	float outvar = 0;
	float kpvar = 0;
	float tivar = 0;
	float tdvar = 0;
	float e = 0;
	float ec = 0;
	float pRes[10][3] = {0};
	float iRes[10][3] = {0};
	float dRes[10][3] = {0};
	int channel = whichChannel - 1;

	float kpfinal = 0;
	float tifinal = 0;
	float tdfinal = 0;

	float a = 0;
	float b = 0;
	float c = 0;

	param[channel].offset1 = cur - target;	
	if (1)
	{
		param[channel].offset1 = -param[channel].offset1;
	}

	param[channel].cur1 = cur;

	//printf("cu1=%f,cu2=%f\n", param[channel].cur1, param[channel].cur2);

	// 模糊化
	e = param[channel].offset1 / param[channel].ratioFactorE;
	ec = (param[channel].cur1 - param[channel].cur2) / param[channel].ratioFactorEC;

	//printf("e=%f,ec=%f\n", e, ec);

	// 规则匹配
	PIDFuzzyRule(e, ec, pRes, P, 10);
	PIDFuzzyRule(e, ec, iRes, I, 10);
	PIDFuzzyRule(e, ec, dRes, D, 10);
	// 解模糊
	kpvar = defuzzy(pRes, 10, P);
	tivar = defuzzy(iRes, 10, I);
	tdvar = defuzzy(dRes, 10, D);

	//printf("kpfz=%f,tifz=%f,tdfz=%f\n", kpvar, tivar, tdvar);

	// // 放大
	// kpvar *= param[channel].ratioFactorP;
	// tivar /= param[channel].ratioFactorI;
	// tdvar /= param[channel].ratioFactorD;
	
	//按原系数处理
	kpvar *= kp/3;
	tivar *= ti/3;
	tdvar *= td/3;

	//printf("kpvar=%f,tivar=%f,tdvar=%f\n", kpvar, tivar, tdvar);

	kpfinal = ((kp + kpvar) > 0) ? (kp + kpvar) : 0;
	tifinal = ((ti + tivar) > 0) ? (ti + tivar) : 1;	//为0时导致除法异常
	tdfinal = ((td + tdvar) > 0) ? (td + tdvar) : 0;

	//printf("kpfinal=%f,tifinal=%f,tdfinal=%f\n", kpfinal, tifinal, tdfinal);

	a = (kpfinal) * (1 + t / (tifinal) + (tdfinal) / t);
	b = (-(kpfinal)) * (1 + 2 * (tdfinal) / t);
	c = (kpfinal) * (tdfinal) / t;
	//printf("a=%f,b=%f,c=%f\n", a, b, c);

	outvar = a * param[channel].offset1 + b * param[channel].offset2 + c * param[channel].offset3;
	param[channel].offset2 = param[channel].offset1;
	param[channel].offset3 = param[channel].offset2;

	param[channel].cur2 = param[channel].cur1;

	//printf("outvar=%f\n", outvar);

	param[channel].pwmout += (int)(-outvar);

	if (param[channel].pwmout >= param[channel].maxout)
		param[channel].pwmout = param[channel].maxout;
	if (param[channel].pwmout <= param[channel].minout)
		param[channel].pwmout = param[channel].minout;
	//printf("outpwm=%d\n", param[channel].pwmout);
	return param[channel].pwmout;
}

#define BUF_LEN 5
int main(void)
{
	float cur = 500;
	int out = 0, target = 240;	//0
	int arry[BUF_LEN]={0}, i=0;
	while (1)
	{
		// scanf("%f", &cur);
		out = outPut(target, cur, 10, 30, 10, 1);
		printf("cur:%f;out:%d\n", cur, out);

		//模拟延时
		for(i=0; i<BUF_LEN-1; i++)
		{
			arry[i] = arry[i+1];
		}
		arry[i] = out;

		cur -= (arry[0]-cur)/100;

		if (target == cur)
		{
			return 0;
		}

		usleep(200000);	//延时200ms
	}
}