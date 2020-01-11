#include <stdio.h>  
#include <stdlib.h>
#include <time.h>  
#include <bitset>
#include <math.h> 
#include <iostream> 
#include <queue>
using namespace std;

typedef struct node{
   double up;//结点的价值上界
   double v;//结点对应的价值
   int w;//结点对应的重量
   int level;//结点在数组中的位置 
}Node;

Node node;
bool operator < (Node a,Node b)
{
    return a.up < b.up;
}
priority_queue<Node> que;

int n,m;//n，m为物品数量和背包体积
int maxv;//最大价值

int *x;//记录选中情况
int *w,*v;//w为物品体积，v为物品价值

int V[100][100];//备忘录

//穷举法
int Exhaustive()  
{  
    int num,maxn=0;//maxv为最大价值，maxn为选择序列 
    maxv = 0; 
    int tempw, tempv;  
    int k;  
  
    for (num=0; num < pow(2,n); num++) //每一个num对应一个解  
    {  
        k=num; 
        tempw=tempv=0;     
        for (int i=0; i<n; i++) //n位二进制  
        {  
            if (k%2 == 1){     //如果相应的位等于1，则代表物体放进去，如果是0，就不用放了  
                tempw += w[i];  
                tempv += v[i];  
            }  
            k = k/2;          //二进制右移一位
        }  
        //循环结束后，一个解生成  
        //判断是否超过了背包的容积
        if (tempw <= m){  
            if (tempv > maxv){  //如果没有超，判断当前解是否比最优解更好  
                maxv = tempv;
                maxn = num;
            }  
        }  
    }  
    cout<< "穷举法选择的序列" << bitset<sizeof(unsigned int)*3>(maxn)<<endl;
    
    return maxv;
}  

//动态规划算法
int Dynamic()
{
    //初始化
    for (int i=0; i<=n; i++)
        for(int j=0; j<=m; j++)
            V[i][j] = 0;

    for (int i=1; i<=n; i++)
    {
        for (int j=1; j<=m; j++)
        {
            if (j < w[i-1]) //j < 第i个物品的体积
                V[i][j]=V[i-1][j];
            else 
                V[i][j]=max(V[i-1][j], V[i-1][j-w[i-1]]+v[i-1]);
        }
    } 

    //判断哪些被选中
    int j=m;
    for (int i=n; i>=1; i--)
    {
        if (V[i][j] > V[i-1][j]) {
            x[i] = 1;
            j = j-w[i-1];
        }
        else
            x[i] = 0;
    }
    cout<<"动态规划选择的序列:";
    for (int i=1; i<=n; ++i)
        cout<<x[i];
    cout<<endl;

    return V[n][m];
}

//自顶向下的备忘录法
int RecordSolve(int i, int j)//前i个物品装入体积为j的包
{
    if (V[i][j] != -1)//已经计算过了
        return V[i][j];

    int result = 0;
    if (i >= n)//i从0到n-1
        return result;

    if (w[i] > j)//装不下
        V[i][j] = RecordSolve(i+1, j);
    else
        V[i][j] = max(RecordSolve(i+1, j), RecordSolve(i+1, j - w[i])+v[i]);

    return V[i][j];
}

int Record()
{
    //初始化
    for (int i=0; i<=n; i++)
        for(int j=0; j<=m; j++)
            V[i][j] = -1;   
    
    return RecordSolve(0, m); 
}

//回溯法：深度优先（递归）
//cw当前包内物品体积，cv当前包内物品价值

//剪枝函数：计算剩余物品的最高价值上界
double bound(int i, int cv, int cw, int* v, int* w) 
{
    double maxLeft = cv;//价值上界
    int wleft = m - cw;//剩余空间

    while (i < n && w[i] <= wleft) 
    {
        wleft -= w[i];
        maxLeft += v[i];
        i++;
    }
    //装不下时，用下一个物品的单位重量价值折算到剩余空间推测
    if (i < n)  maxLeft += v[i] / w[i] * wleft;
    return maxLeft;
}

void DfsTraceback(int i, int cv, int cw, int* tmp) 
{
    if (i >= n) {//回溯结束
        if (cv > maxv) {
            maxv = cv;
            for (int i = 0; i <= n; ++i) x[i] = tmp[i];
        }
        return;
    } 

    //装入物品，搜索左边结点
    if (cw + w[i] <= m) {
        cw += w[i];
        cv += v[i];
        tmp[i] = 1;
        DfsTraceback(i+1, cv, cw, tmp);
        cw -= w[i];
        cv -= v[i];
        tmp[i] = 0;
    }
    //不装入这个物品，直接搜索右边的结点
    if (bound(i+1, cv, cw, v, w) >= maxv) DfsTraceback(i+1, cv, cw, tmp);
}

int Dfs()
{
    int tmp[n+1];//一种解，暂存一种选择情况

    //初始化
    for (int i=0; i<=n; i++) tmp[i]=0;
    maxv=0;

    DfsTraceback(0,0,0,tmp);

    cout<<"回溯法选择的序列:";
    for(int i=0; i<n; i++) cout<<x[i];
    cout<<endl;

    return maxv;
}

//排序(降序)
void Sort(double* v_w, int* v, int* w)//n为数组a的元素个数
{
    //进行N-1轮选择
    for(int i=0; i<n-1; i++)
    {
        int max_index = i; 
        //找出第i大的数所在的位置
        for(int j=i+1; j<n; j++)
            if(v_w[j] > v_w[max_index])
                max_index = j;
        //将第i小的数，放在第i个位置；如果刚好，就不用交换
        if( i != max_index)
        {
            double tempv_w = v_w[i];
            int tempw = w[i], tempv = v[i];
            v_w[i] = v_w[max_index];
            v[i] = v[max_index]; 
            w[i] = w[max_index]; 
            v_w[max_index] = tempv_w;
            v[max_index] = tempv;
            w[max_index] = tempw;
        }
    }
}

//分支限界法
void addLiveNode(double up, int cv, int cw, int level)
{
    node.up = up;
    node.v = cv;
    node.w = cw;
    node.level = level;
    if (level <= n)  que.push(node);
}

void BfsTraceback(int* v, int* w) 
{
    int i = 0;
    int cv = 0, cw = 0;
    double up = bound(i, cv, cw, v, w); //计算价值上界
    while (i != n) 
    {
        //检查当前扩展结点的左结点是否可行
        if (cw + w[i] <= m) {
            if (cv + v[i] > maxv)   maxv = cv + v[i];
            //将左孩子结点插入到优先队列中
            addLiveNode(up, cv+v[i], cw+w[i], i+1);
        }
        up = bound(i+1, cv, cw, v, w);
        if (up > maxv) {//右子树可能有最优解
            //将右孩子结点插入到优先队列
            addLiveNode(up, cv, cw, i+1);
        }
        if (que.empty()) 
            return;
        //从优先队列中取下一个扩展结点N
        Node node = que.top();
        que.pop();
        //cout<<node.v<<" ";
        cw = node.w;
        cv = node.v;
        up = node.up;
        i = node.level;
    }
}

int Bfs()
{
    int tmp[n+1];//一种解，暂存一  种选择情况
    int tmpw[n], tmpv[n];
    double tmpv_w[n];//存储重新排序的v[n]/w[n]和w[n]

    //初始化
    for (int i=0; i<=n; i++) tmp[i]=0;
    maxv=0;
    for (int i=0; i<n; i++) tmpv_w[i] = (double)v[i]/w[i];
    for (int i=0; i<n; i++) tmpv[i] = v[i];
    for (int i=0; i<n; i++) tmpw[i] = w[i];

    //物品按照单位价值率排序
    Sort(tmpv_w, tmpv, tmpw);
    BfsTraceback(tmpv,tmpw);
    return maxv;
}

//蒙特卡罗算法
int MonteCarlo()
{
    int k, tempw, tempv, maxn=0;

    maxv=0;
    srand((unsigned)time(NULL));
    for (int i=0; i<1000; i++)
    {
        k=rand() % (int)pow(2,n); 
        tempw=tempv=0;     
        for (int i=0; i<n; i++) //n位二进制  
        {  
            if (k%2 == 1){     //如果相应的位等于1，则代表物体放进去，如果是0，就不用放了  
                tempw += w[i];  
                tempv += v[i];  
            }  
            k = k/2;          //二进制右移一位
        }  
        //循环结束后，一个解生成  
        //判断是否超过了背包的容积
        if (tempw <= m){  
            if (tempv > maxv){  //如果没有超，判断当前解是否比最优解更好  
                maxv = tempv;
                maxn = k;
            }  
        }  
    }
    return maxv;
}


int main()
{
    FILE* fp;
    if ((fp = fopen("input.txt", "r")) == NULL) {
        cout<<"打开失败"<<endl;
        exit(0);
    }

    fscanf(fp, "%d\n%d", &n, &m);

    w=new int [n];  
    v=new int [n];
    x=new int [n+1];

    for (int i=0; i<n; i++)
        fscanf(fp, "%d", &w[i]);
    for (int i=0; i<n; i++)
        fscanf(fp, "%d", &v[i]);    

    if ((fp = fopen("output.txt", "w+")) == NULL) {
        cout<<"新建失败"<<endl;
        exit(0);
    }

    fprintf(fp, "穷举法最大价值为 %d\n", Exhaustive());
    fprintf(fp, "动态规划最大价值为 %d\n", Dynamic());
    fprintf(fp, "自顶向下的备忘录法最大价值为 %d\n", Record());
    fprintf(fp, "回溯法的最大价值为 %d\n", Dfs());
    fprintf(fp, "分支限界法的最大价值为 %d\n", Bfs());
    fprintf(fp, "蒙特卡罗算法的最大价值为 %d\n", MonteCarlo());




    /*cout<<"输入物品数量和背包体积:";  
    scanf("%d%d",&n,&m);  
    w=new int [n];  
    v=new int [n];
    x=new int [n+1];

  
    cout<<"输入"<< n <<"个体积:";  
    for(int i=0;i<n;i++)  
        scanf("%d",&w[i]);  
  
    cout<<"输入"<< n <<"个价值:";  
    for(int i=0;i<n;i++)  
        scanf("%d",&v[i]);  
  
    int choose = 1;
    while(choose)
    {
        cout<<"--------------------"<<endl;
        cout<<"0. 退出"<<endl;
        cout<<"1. 穷举法"<<endl;
        cout<<"2. 动态规划"<<endl;
        cout<<"3. 自顶向下的备忘录法"<<endl;
        cout<<"4. 回溯法"<<endl;

        cout<<"选择：";
        cin>>choose;
        cout<<"--------------------"<<endl;
        switch(choose)
        {
            case 0: exit(0);break;
            case 1: printf("穷举法最大价值为%d\n", Exhaustive()); break;
            case 2: printf("动态规划最大价值为%d\n", Dynamic()); break;
            case 3: printf("自顶向下的备忘录法最大价值为%d\n", Record()); break;
            case 4: printf("回溯法的最大价值为%d\n", Bfs());break;        


            default: break;
        }
    }*/     
}