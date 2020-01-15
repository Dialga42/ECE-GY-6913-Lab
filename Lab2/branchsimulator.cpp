#include <bits/stdc++.h>
using namespace std;
const int M=1<<20,N=1<<8;
bool PHC[M][N][2];
int main(int argc,char* argv[])
{
	int m,k,pc,taken,now;
	freopen(argv[1],"r",stdin);
	scanf("%d%d",&m,&k);
	now=(1<<k)-1;
	freopen(argv[2],"r",stdin);
	freopen((string(argv[2])+".out").c_str(),"w",stdout);
	memset(PHC,1,sizeof PHC);
	while (~scanf("%x%d",&pc,&taken))
	{
		bool (&tmp)[2]=PHC[pc&((1<<m)-1)][now];
		printf("%d\n",tmp[0]==1);
		tmp[1]==taken?tmp[0]=taken:tmp[1]=taken;
		if (k)
			now=(now>>1)^(taken*(1<<(k-1)));
	}
	return 0;
}